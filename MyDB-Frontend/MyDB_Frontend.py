# main.py (ASCII only)

import sys, shlex
from pathlib import Path

root = Path(__file__).resolve().parents[1]
candidates = [
    root / "MyDB-Backend" / "build" / "Release",
    root / "MyDB-Backend" / "build" / "Debug",
    Path.cwd(),
]
for p in candidates:
    if p.exists():
        sys.path.insert(0, str(p))

try:
    import mydb
except Exception:
    print("mydb module not found. Build the backend first.")
    print("Searched:")
    for p in candidates:
        print(" -", p)
    raise

db = mydb.DB()

HELP = """Commands:
  ct <table> <schema>          -> create table
     ex: ct Users ID:int,NAME:string,AGE:int
  ins <table> <key> <v1> ...   -> insert row
     ex: ins Users 1 Alice 23
  get <table> <key>            -> get row
     ex: get Users 1
  print <table>                -> print table
  list                         -> list tables
  schema <table>               -> show schema
  help                         -> help text
  exit                         -> quit
"""

def parse_schema(spec: str):
    cols = []
    for part in spec.split(","):
        part = part.strip()
        if not part:
            continue
        if ":" not in part:
            raise ValueError(f"type missing: {part!r} (ex: NAME:string)")
        name, typ = part.split(":", 1)
        cols.append((name.strip(), typ.strip()))
    if not cols:
        raise ValueError("empty schema")
    return cols

def cmd_create(args):
    if len(args) < 2:
        print("usage: ct <table> <schema>")
        return
    table = args[0]
    schema_spec = " ".join(args[1:])
    sch = parse_schema(schema_spec)
    db.create_table(table, sch)
    print(f"created table {table}: {sch}")

def cmd_insert(args):
    if len(args) < 3:
        print("usage: ins <table> <key> <v1> ...")
        return
    table = args[0]
    key = int(args[1])
    row = args[2:]
    db.insert_row(table, key, row)
    print(f"inserted into {table} key={key} row={row}")

def cmd_get(args):
    if len(args) != 2:
        print("usage: get <table> <key>")
        return
    table = args[0]
    key = int(args[1])
    r = db.get_row(table, key)
    print(r if r is not None else None)

def cmd_print(args):
    if len(args) != 1:
        print("usage: print <table>")
        return
    db.print_table(args[0])

def cmd_list(_args):
    print(db.list_tables())

def cmd_schema(args):
    if len(args) != 1:
        print("usage: schema <table>")
        return
    print(db.schema(args[0]))

COMMANDS = {
    "ct":     cmd_create,
    "ins":    cmd_insert,
    "get":    cmd_get,
    "print":  cmd_print,
    "list":   cmd_list,
    "schema": cmd_schema,
    "help":   lambda _: print(HELP),
}

def main():
    print("MyDB CLI. Type 'help', 'exit' to quit.")
    print(HELP)
    while True:
        try:
            line = input("> ").strip()
        except (EOFError, KeyboardInterrupt):
            print()
            break
        if not line:
            continue
        if line.lower() in ("exit", "quit"):
            break
        try:
            parts = shlex.split(line)
        except ValueError as e:
            print("parse error:", e)
            continue
        cmd, *args = parts
        func = COMMANDS.get(cmd.lower())
        if not func:
            print("unknown command. type 'help'.")
            continue
        try:
            func(args)
        except Exception as e:
            print("error:", e)

if __name__ == "__main__":
    main()
