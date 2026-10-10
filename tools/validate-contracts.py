from pathlib import Path
import json
from jsonschema import Draft202012Validator
root = Path(__file__).resolve().parents[1]
for kind, example in (("dialog", "dialogs/open-media.json"), ("plugin", "plugins/tracker.json")):
    schema = json.loads((root / "schemas" / f"{kind}.schema.json").read_text())
    Draft202012Validator.check_schema(schema)
    validator = Draft202012Validator(schema)
    value = json.loads((root / "assets" / example).read_text())
    validator.validate(value)
    invalid = dict(value, shell="execute arbitrary command")
    assert not validator.is_valid(invalid), "Unexpected executable property accepted"
    invalid = dict(value, schema_version=999)
    assert not validator.is_valid(invalid), "Unknown schema version accepted"
    if kind == "dialog":
        invalid = dict(value, actions=[dict(value["actions"][0], action="run_shell")])
        assert not validator.is_valid(invalid), "Unregistered action accepted"
        ids = [field["id"] for field in value["fields"]]
        assert len(ids) == len(set(ids)), "Duplicate field IDs"
    print(f"{kind}: schema/example and negative cases passed")
