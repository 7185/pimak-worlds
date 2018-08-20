# How to contribute

We need your help!

You can submit a bug report or find feature requests under [Issues](https://github.com/7185/pimak-worlds/issues).

## Submitting changes

Please send a [Pull Request to pimak-worlds](https://github.com/7185/pimak-worlds/pull/new/master) with a clear list of what you've done (read more about [pull requests](http://help.github.com/pull-requests/)).
Please follow our coding conventions (below) and make sure all of your commits are atomic (one feature per commit).
We use [conventional commits](https://conventionalcommits.org/).

Always rebase your branch if the master branch has newer code:

    $ git checkout -b feature-branch
    $ git checkout feature-branch
    (...edit, add and commit files...)
    $ git fetch https://github.com/7185/pimak-worlds.git
    $ git rebase FETCH_HEAD
    $ git push

Note: if you already pushed your new branch, just do `git push -f` to overwrite it.

## Coding conventions

We try to follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
