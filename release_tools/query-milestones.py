#!/usr/bin/python3

import argparse as ap

import shared


ACTIONS = dict()


def action(key):
    def wrapper(function):
        ACTIONS[key] = function
        return function
    return wrapper


def get_closed_issues(repo, milestone):
    issues_and_prs = repo.get_issues(milestone=milestone, state="closed")
    issues_only = [i for i in issues_and_prs if i.pull_request is None]
    return issues_only


def get_closed_prs(repo, milestone):
    issues_and_prs = repo.get_issues(milestone=milestone, state="closed")
    prs_only = [i for i in issues_and_prs if i.pull_request is not None]
    return prs_only


@action("issues-closed")
def print_closed_issues(repo, milestone):
    for issue in get_closed_issues(repo, milestone):
        print(issue.title)


@action("prs-merged")
def print_closed_prs(repo, milestone):
    for pr in get_closed_prs(repo, milestone):
        print(pr.title)


def create_parser():
    parser = ap.ArgumentParser()
    parser.add_argument("version", type=shared.version_type)
    parser.add_argument("what", choices=(ACTIONS.keys()))
    shared.update_parser_with_common_stuff(parser)
    return parser


if __name__ == "__main__":
    parser = create_parser()
    args = parser.parse_args()
    gh = shared.get_github(args)
    repo = shared.get_repo(gh, "OpenSCAP")

    milestone = shared.get_milestone(repo, args.version)
    ACTIONS[args.what](repo, milestone)
