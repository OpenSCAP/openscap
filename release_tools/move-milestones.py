#!/usr/bin/python3

import argparse as ap

import shared


def create_parser():
    parser = ap.ArgumentParser()
    parser.add_argument("old-version", type=shared.version_type)
    parser.add_argument("new-version", type=shared.version_type)
    shared.update_parser_with_common_stuff(parser)
    return parser


def create_new_milestone(repo, name):
    assert shared.get_milestone(repo, name) is None, \
        f"There already is a milestone {name}"
    return repo.create_milestone(name, "open")


def transfer_open_issues_and_prs_to_new_milestone(repo, old_milestone, new_milestone):
    assert old_milestone.title != new_milestone.title, \
        f"The new and old milestones have the same title '{new_milestone.title}'"
    old_milestone_issues = repo.get_issues(milestone=old_milestone, state="open")

    for issue in old_milestone_issues:
        issue.edit(milestone=new_milestone)


def close_milestone(milestone):
    milestone.edit(milestone.title, state="closed")


if __name__ == "__main__":
    parser = create_parser()
    args = parser.parse_args()
    github = shared.get_github(args)
    repo = shared.get_repo(github, args.owner)

    old_milestone = shared.get_milestone(repo, vars(args)["old-version"])
    new_milestone = create_new_milestone(repo, vars(args)["new-version"])
    transfer_open_issues_and_prs_to_new_milestone(repo, old_milestone, new_milestone)
    close_milestone(old_milestone)
