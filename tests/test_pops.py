"""Testes de integracao das funcionalidades 11 a 14, sem depender de 8 a 10.

Uso: python tests/test_pops.py --program caminho/do/programaTrab
Os binarios de entrada sao montados diretamente a partir do contrato do arquivo.
Cada teste usa uma pasta temporaria e confere a saida e o conteudo dos arquivos.
"""

import argparse
import csv
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
HEADER = struct.Struct("<cqqii")
CONTROL = struct.Struct("<ciq")
FIXED = struct.Struct("<ciqi2s")
INDEX = struct.Struct("<iq")
FAILURE = "Falha no processamento do arquivo.\n"
MISSING = "Registro inexistente.\n"
PROGRAM = None


class PoPsTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="trabalhoed3-pops-")
        self.addCleanup(self.temp.cleanup)
        self.folder = Path(self.temp.name)
        self.data = self.folder / "dados.bin"
        self.index = self.folder / "indice.bin"

    def create(self, rows=(), removed=(), spaces=None):
        """removed especifica a ordem dos IDs na lista, nao a ordem fisica."""
        spaces = spaces or {}
        offsets, sizes = {}, {}
        pos = HEADER.size
        for id_, name, country, code in rows:
            offsets[id_] = pos
            sizes[id_] = spaces.get(id_, 21 + len(name.encode()) + len(country.encode()))
            pos += sizes[id_]
        links = {
            id_: offsets[removed[i + 1]] if i + 1 < len(removed) else -1
            for i, id_ in enumerate(removed)
        }
        raw = bytearray(HEADER.pack(b"1", offsets[removed[0]] if removed else -1,
                                    pos, len(removed), len(rows) - len(removed)))
        for id_, name, country, code in rows:
            size = sizes[id_]
            if id_ in links:
                raw += CONTROL.pack(b"1", size, links[id_]) + b"$" * (size - CONTROL.size)
            else:
                record = FIXED.pack(b"0", size, -1, id_, code.encode() if code else b"$$")
                record += name.encode() + b"|" + country.encode() + b"|"
                self.assertLessEqual(len(record), size)
                raw += record + b"$" * (size - len(record))
        self.data.write_bytes(raw)
        active = sorted((id_, offset) for id_, offset in offsets.items() if id_ not in links)
        self.index.write_bytes(b"1" + b"".join(INDEX.pack(*entry) for entry in active))
        return offsets

    def run_program(self, option, requests, body="", success=True):
        result = subprocess.run(
            [str(PROGRAM)], input=f"{option} dados.bin indice.bin {requests}\n{body}",
            cwd=self.folder, text=True, encoding="utf-8", capture_output=True, timeout=10,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stderr, "")
        if success and option in (12, 13, 14):
            expected = "".join(f"{sum(path.read_bytes()) / 100:.6f}\n" for path in (self.data, self.index))
            self.assertEqual(result.stdout, expected)
        return result.stdout

    def inspect(self):
        """Verifica limites, lixo, contadores, encadeamentos e correspondencia exata do indice."""
        raw = self.data.read_bytes()
        status, top, end, removed_count, count = HEADER.unpack_from(raw)
        self.assertEqual(status, b"1")
        self.assertEqual(end, len(raw))
        pos, records, active = HEADER.size, {}, {}
        while pos < end:
            flag, size, link = CONTROL.unpack_from(raw, pos)
            self.assertGreaterEqual(size, 21)
            self.assertLessEqual(pos + size, end)
            self.assertIn(flag, (b"0", b"1"))
            record = {"flag": flag, "size": size, "link": link, "offset": pos}
            if flag == b"1":
                self.assertEqual(raw[pos + CONTROL.size:pos + size], b"$" * (size - CONTROL.size))
            else:
                self.assertEqual(link, -1)
                _, _, _, id_, code = FIXED.unpack_from(raw, pos)
                name, country, padding = raw[pos + FIXED.size:pos + size].split(b"|", 2)
                self.assertEqual(padding, b"$" * len(padding))
                self.assertNotIn(id_, active)
                record.update(id=id_, name=name.decode(), country=country.decode(),
                              code="" if code == b"$$" else code.decode())
                active[id_] = record
            records[pos] = record
            pos += size
        self.assertEqual(pos, end)
        chain = []
        while top != -1:
            self.assertNotIn(top, chain, "ciclo na lista de removidos")
            self.assertIn(top, records)
            self.assertEqual(records[top]["flag"], b"1")
            chain.append(top)
            top = records[top]["link"]
        self.assertEqual(set(chain), {p for p, r in records.items() if r["flag"] == b"1"})
        self.assertEqual(removed_count, len(chain))
        self.assertEqual(count, len(active))
        index = self.index.read_bytes()
        self.assertEqual(index[:1], b"1")
        entries = list(INDEX.iter_unpack(index[1:]))
        self.assertEqual(entries, sorted((id_, r["offset"]) for id_, r in active.items()))
        return active, chain

    def test_csv_queries_indexed_sequential_and_null(self):
        with (ROOT / "Source" / "PoPs.csv").open(encoding="utf-8-sig", newline="") as source:
            rows = [(int(r["idPoPs"]), r["nomePoPs"], r["nomePais"], r["siglaPais"])
                    for r in csv.DictReader(source)]
        self.create(rows)
        original = self.data.read_bytes(), self.index.read_bytes()
        body = "".join(f"1 idPoPs {r[0]}\n" for r in rows)
        expected = "".join(f"{id_} {name or 'NULO'} {country or 'NULO'} {code or 'NULO'}\n"
                           for id_, name, country, code in rows)
        self.assertEqual(self.run_program(11, len(rows), body), expected)
        cases = [("nomePais", "BRAZIL", 2), ("nomePoPs", "", 1), ("siglaPais", "", 3)]
        body = "".join(f'1 {key} ' + (f'"{value}"' if value else "NULO") + "\n"
                       for key, value, _ in cases)
        expected = ""
        for _, value, col in cases:
            matches = [row for row in rows if row[col] == value]
            expected += "".join(f"{id_} {name or 'NULO'} {country or 'NULO'} {code or 'NULO'}\n"
                                for id_, name, country, code in matches) or MISSING
        self.assertEqual(self.run_program(11, len(cases), body), expected)
        self.assertEqual((self.data.read_bytes(), self.index.read_bytes()), original)
        self.inspect()

    def test_multiple_criteria_id_not_first_and_missing(self):
        self.create([(7, "CAMPINA GRANDE", "BRAZIL", "BR"), (2, "PARIS", "FRANCE", "FR")])
        body = ('2 nomePais "BRAZIL" idPoPs 7\n'
                '2 idPoPs 7 nomePais "FRANCE"\n'
                '1 idPoPs 999\n1 idPoPs 2\n')
        self.assertEqual(self.run_program(11, 4, body),
                         "7 CAMPINA GRANDE BRAZIL BR\n" + MISSING * 2 + "2 PARIS FRANCE FR\n")

    def test_indexed_query_does_not_scan_unrelated_record(self):
        offsets = self.create([(1, "A", "B", "BR"), (2, "X", "Y", "CL")])
        raw = bytearray(self.data.read_bytes())
        raw[offsets[1] + FIXED.size:offsets[2]] = b"?" * (offsets[2] - offsets[1] - FIXED.size)
        self.data.write_bytes(raw)
        self.assertEqual(self.run_program(11, 1, "1 idPoPs 2\n"), "2 X Y CL\n")
        self.assertEqual(self.run_program(11, 1, '1 nomePais "Y"\n'), FAILURE)

    def test_delete_multiple_requests_no_match_and_repeated_match(self):
        self.create([(1, "A", "BRAZIL", "BR"), (2, "B", "BRAZIL", "BR"), (3, "C", "CHILE", "CL")])
        self.run_program(12, 4, '1 idPoPs 999\n1 nomePais "BRAZIL"\n1 idPoPs 1\n1 idPoPs 3\n')
        active, chain = self.inspect()
        self.assertEqual(active, {})
        self.assertEqual(len(chain), 3)
        self.assertEqual(self.index.read_bytes(), b"1")
        self.assertEqual(self.run_program(11, 1, '1 nomePais "BRAZIL"\n'), MISSING)

    def test_worst_fit_unordered_list_middle_and_tail(self):
        offsets = self.create([(1, "A", "", ""), (2, "B", "", ""), (3, "C", "", "")],
                              removed=(1, 2, 3), spaces={1: 24, 2: 45, 3: 30})
        end = len(self.data.read_bytes())
        self.run_program(13, 2, '10 "NEW" NULO "BR"\n11 NULO NULO NULO\n')
        active, chain = self.inspect()
        self.assertEqual(active[10]["offset"], offsets[2])
        self.assertEqual(active[10]["size"], 45)
        self.assertEqual(active[11]["offset"], offsets[3])
        self.assertEqual(chain, [offsets[1]])
        self.assertEqual(len(self.data.read_bytes()), end)

    def test_worst_fit_tie_first_in_chain_and_exact_fit(self):
        offsets = self.create([(1, "AAA", "", ""), (2, "BBB", "", "")],
                              removed=(2, 1))
        self.run_program(13, 1, '9 "XYZ" NULO NULO\n')
        active, chain = self.inspect()
        self.assertEqual(active[9]["offset"], offsets[2])
        self.assertEqual(active[9]["size"], 24)
        self.assertEqual(chain, [offsets[1]])

    def test_insert_appends_if_largest_hole_is_too_small(self):
        offsets = self.create([(1, "A", "", "")], removed=(1,))
        end = len(self.data.read_bytes())
        self.run_program(13, 1, '2 "A LONG NAME" "BRAZIL" "BR"\n')
        active, chain = self.inspect()
        self.assertEqual(active[2]["offset"], end)
        self.assertEqual(chain, [offsets[1]])

    def test_delete_pushes_at_head_and_insert_reuses_it(self):
        offsets = self.create([(1, "AA", "", ""), (2, "BB", "", "")])
        self.run_program(12, 2, "1 idPoPs 1\n1 idPoPs 2\n")
        _, chain = self.inspect()
        self.assertEqual(chain, [offsets[2], offsets[1]])
        self.run_program(13, 2, '3 NULO NULO NULO\n4 NULO NULO NULO\n')
        active, chain = self.inspect()
        self.assertEqual(active[3]["offset"], offsets[2])
        self.assertEqual(active[4]["offset"], offsets[1])
        self.assertEqual(chain, [])

    def test_update_shrink_then_grow_inside_existing_space_and_change_id(self):
        offsets = self.create([(1, "LONG NAME", "BRAZIL", "BR")])
        end = len(self.data.read_bytes())
        self.run_program(14, 3, '1 idPoPs 1\n2 nomePoPs NULO nomePais NULO\n'
                         '1 idPoPs 1\n2 nomePoPs "NEW NAME" idPoPs 8\n'
                         '1 idPoPs 8\n1 siglaPais NULO\n')
        active, chain = self.inspect()
        self.assertEqual(set(active), {8})
        self.assertEqual(active[8]["offset"], offsets[1])
        self.assertEqual(active[8]["name"], "NEW NAME")
        self.assertEqual(active[8]["country"], "")
        self.assertEqual(active[8]["code"], "")
        self.assertEqual(chain, [])
        self.assertEqual(len(self.data.read_bytes()), end)

    def test_update_relocation_uses_worst_fit_and_updates_index(self):
        offsets = self.create([(1, "A", "", ""), (2, "B", "", ""), (3, "C", "", "")],
                              removed=(3, 2), spaces={2: 60, 3: 50})
        self.run_program(14, 1, '1 idPoPs 1\n3 idPoPs 10 nomePoPs "A LONG NAME" nomePais "BRAZIL"\n')
        active, chain = self.inspect()
        self.assertEqual(set(active), {10})
        self.assertEqual(active[10]["offset"], offsets[2])
        self.assertEqual(active[10]["size"], 60)
        self.assertEqual(chain, [offsets[1], offsets[3]])
        self.assertEqual(self.run_program(11, 2, '1 idPoPs 1\n1 idPoPs 10\n'),
                         MISSING + "10 A LONG NAME BRAZIL NULO\n")

    def test_update_many_relocations_and_next_request_sees_changes(self):
        self.create([(1, "A", "B", "BR"), (2, "C", "B", "BR")])
        self.run_program(14, 3, '1 idPoPs 999\n1 nomePais "IGNORED"\n'
                         '1 nomePais "B"\n1 nomePais "A MUCH LONGER COUNTRY NAME"\n'
                         '1 nomePais "A MUCH LONGER COUNTRY NAME"\n1 siglaPais "CL"\n')
        active, chain = self.inspect()
        self.assertEqual(set(active), {1, 2})
        self.assertEqual(len(chain), 2)
        for record in active.values():
            self.assertEqual(record["country"], "A MUCH LONGER COUNTRY NAME")
            self.assertEqual(record["code"], "CL")

    def test_long_strings_and_vectors_larger_than_one_block(self):
        self.create()
        name = "A" * 200 + " B" * 50
        self.run_program(13, 130, "".join(f'{id_} "{name}" NULO NULO\n' for id_ in range(130, 0, -1)))
        active, _ = self.inspect()
        self.assertEqual(len(active), 130)
        self.assertTrue(all(record["name"] == name for record in active.values()))
        self.run_program(14, 1, '1 nomePais NULO\n1 nomePais "CHILE"\n')
        active, _ = self.inspect()
        self.assertTrue(all(record["country"] == "CHILE" for record in active.values()))
        self.run_program(12, 1, '1 nomePais "CHILE"\n')
        active, chain = self.inspect()
        self.assertEqual(active, {})
        self.assertEqual(len(chain), 260)

    def test_empty_files_and_zero_requests(self):
        self.create()
        self.assertEqual(self.run_program(11, 1, "1 idPoPs 5\n"), MISSING)
        for option in (12, 13, 14):
            self.run_program(option, 0)
            self.assertEqual(self.inspect(), ({}, []))

    def test_inconsistent_status_rejected_before_other_fields(self):
        for path in (self.data, self.index):
            for option in (11, 12, 13, 14):
                with self.subTest(path=path.name, option=option):
                    self.create()
                    path.write_bytes(b"0")
                    before = self.data.read_bytes(), self.index.read_bytes()
                    self.assertEqual(self.run_program(option, 0, success=False), FAILURE)
                    self.assertEqual((self.data.read_bytes(), self.index.read_bytes()), before)

    def test_truncated_index_and_data_report_failure(self):
        self.create([(1, "ABC", "BRAZIL", "BR")])
        self.index.write_bytes(self.index.read_bytes()[:-1])
        self.assertEqual(self.run_program(11, 1, "1 idPoPs 1\n"), FAILURE)
        self.create([(1, "ABC", "BRAZIL", "BR")])
        self.data.write_bytes(self.data.read_bytes()[:-1])
        self.assertEqual(self.run_program(11, 1, "1 idPoPs 1\n"), FAILURE)

    def test_duplicate_ids_do_not_get_written(self):
        self.create([(1, "A", "", ""), (2, "B", "", "")])
        payload = self.data.read_bytes()[HEADER.size:]
        self.assertEqual(self.run_program(13, 1, '1 "DUPLICATE" NULO NULO\n', success=False), FAILURE)
        self.assertEqual(self.data.read_bytes()[HEADER.size:], payload)
        self.assertEqual(self.data.read_bytes()[:1], b"0")
        self.assertEqual(self.index.read_bytes()[:1], b"0")
        self.create([(1, "A", "", ""), (2, "B", "", "")])
        self.assertEqual(self.run_program(14, 1, '1 idPoPs 1\n1 idPoPs 2\n', success=False), FAILURE)
        self.assertEqual(self.data.read_bytes()[HEADER.size:], payload)

    def test_invalid_input_rejected(self):
        self.create()
        self.assertEqual(self.run_program(11, 1, "1 campoInvalido 2\n"), FAILURE)
        self.assertEqual(self.run_program(11, 1, "1 idPoPs 12abc\n"), FAILURE)
        self.assertEqual(self.run_program(11, 1, '1 nomePoPs "sem fechar\n'), FAILURE)
        self.assertEqual(self.run_program(11, 1, "1 idPoPs\n"), FAILURE)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--program", required=True, type=Path)
    args, remaining = parser.parse_known_args()
    PROGRAM = args.program.resolve(strict=True)
    unittest.main(argv=[__file__, *remaining], verbosity=2)
