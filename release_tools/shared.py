import argparse as ap

import github


def update_parser_with_common_stuff(parser):
    parser.add_argument("--owner", default="openscap")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--auth-token")


def get_milestone(repo, name):
    milestones = repo.get_milestones()
    matches = [m for m in milestones if m.title == name]
    assert len(matches) <= 1, \
        f"Expected to find at most one milestone {name}, found {len(matches)}"
    if len(matches) == 0:
        return None
    else:
        return matches[0]


def get_github(args):
    if args.auth_token is not None:
        return github.Github(args.auth_token)
    else:
        assert 0, "Credentials were not supplied"


def get_repo(github, owner):
    repo = github.get_repo(f"{owner}/openscap")
    return repo


def version_type(string):
    components = string.split(".")
    shortest_component_len = min([len(x) for x in components])
    if len(components) != 3 or shortest_component_len == 0:
        msg = (
            "Expected version number of form X.Y.Z, where X, Y, Z are strings. "
            f"Got: '{string}'"
        )
        raise ap.ArgumentTypeError(msg)
    return string
