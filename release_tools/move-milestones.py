#!/usr/bin/env python3

import github
import argparse as ap


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


def create_parser():
    parser = ap.ArgumentParser()
    parser.add_argument("old-version", type=version_type)
    parser.add_argument("new-version", type=version_type)
    parser.add_argument("--owner", default="openscap")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--auth-token")
    return parser


def get_github(args):
    if args.auth_token is not None:
        return github.Github(args.auth_token)
    else:
        assert 0, "Credentials were not supplied"


def get_repo(github, owner):
    repo = github.get_repo(f"{owner}/openscap")
    return repo


def get_milestone(repo, name):
    milestones = repo.get_milestones()
    matches = [m for m in milestones if m.title == name]
    assert len(matches) <= 1, \
        f"Expected to find at most one milestone {name}, found {len(matches)}"
    if len(matches) == 0:
        return None
    else:
        return matches[0]


def create_new_milestone(repo, name):
    assert get_milestone(repo, name) is None, \
        f"There already is a milestone {name}"
    return repo.create_milestone(name, "open")


def transfer_open_issues_to_new_milestone(repo, old_milestone, new_milestone):
    assert old_milestone.title != new_milestone.title, \
        f"The new and old milestones have the same title '{new_milestone.title}'"
    open_issues = [issue for issue in repo.get_issues()
                   if issue.state == "open"]
    old_milestone_issues = [issue for issue in open_issues
                            if issue.milestone.number == old_milestone.number]

    for issue in old_milestone_issues:
        issue.edit(milestone=new_milestone)


def close_milestone(milestone):
    milestone.edit(milestone.title, state="closed")


if __name__ == "__main__":
    parser = create_parser()
    args = parser.parse_args()
    github = get_github(args)
    repo = get_repo(github, args.owner)

    old_milestone = get_milestone(repo, vars(args)["old-version"])
    new_milestone = create_new_milestone(repo, vars(args)["new-version"])
    transfer_open_issues_to_new_milestone(repo, old_milestone, new_milestone)
    close_milestone(old_milestone)
