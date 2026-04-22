# Contributing to JustRunThis

Thank you for your interest in contributing! This document explains the branch strategy and pull-request workflow we follow to keep the project organized and easy to review.

---

## Branch strategy

All contributions target `main` through a short-lived branch. Pick the prefix that matches what you are doing:

| Prefix | When to use | Example |
|---|---|---|
| `feature/*` | New functionality or UI change | `feature/interactive-mode` |
| `bugfix/*` | Fixes a regression or incorrect behavior | `bugfix/timeout-not-killing-process` |
| `housekeeping/*` | Refactoring, dependency bumps, CI config, docs | `housekeeping/update-qt-version` |
| `release/*` | Preparing a versioned release (maintainers only) | `release/v1.2.0` |

> Only `release/*` branches are created by maintainers. All other prefixes are open to contributors.

---

## Step-by-step workflow

### 1. Fork and clone

```bash
git clone https://github.com/<your-handle>/justrunthis.git
cd justrunthis
git remote add upstream https://github.com/CABA1998/justrunthis.git
```

### 2. Sync with upstream before starting

```bash
git checkout main
git pull upstream main
```

### 3. Create your branch

Use the appropriate prefix and a short, descriptive slug:

```bash
git checkout -b feature/my-new-thing
```

### 4. Make your changes

- Keep commits focused — one logical change per commit.
- Write a clear commit message that says *why*, not just *what*.
- Make sure the project builds and runs before opening a PR (see [Build instructions](../README.md#build-and-run-on-windows)).

### 5. Push and open a Pull Request

```bash
git push origin feature/my-new-thing
```

Then open a PR on GitHub targeting `main`. Fill in the PR description:

- **What** changed and **why**.
- Steps to test the change manually.
- Screenshots if the UI was modified.

---

## PR review expectations

- A maintainer will review within a few days.
- Address review comments with new commits — do not force-push after the review starts.
- Once approved, the maintainer squash-merges into `main`.

---

## Code style

- C++17, compiled with `-Wall -Wextra`.
- Follow the existing naming conventions (PascalCase for classes, camelCase for methods and variables).
- No new third-party libraries without prior discussion in an issue.
- Qt Widgets only — no QtQuick/QML.

---

## Reporting bugs and requesting features

Open a GitHub Issue before starting any significant work. This avoids duplicated effort and lets us align on the approach before you invest time coding.

---

## License

By contributing you agree that your work will be released under the project's [MIT License](../LICENSE).
