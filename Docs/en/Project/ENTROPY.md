# Entropy Versioning

Cosmeron uses **Entropy** as a measure of accumulated change to the library.

> **Entropy measures accumulated change, not code size.**

A released state is written as:

```text
Entropy: <absolute H><Greek band>
```

For example:

```text
Entropy: 18866β
```

The number is the absolute accumulated entropy in **H**. The Greek suffix identifies the entropy band.

## Canonical scope

The Cosmeron source scope is:

```text
Codespace/Cosmeron/**
```

Tests, documentation, translations, CI configuration, generated badges, and repository metadata do not affect Entropy.

## Origin

The first canonical Cosmeron release establishes the immutable Git tag:

```text
entropy-origin
```

Every tracked line in `Codespace/Cosmeron/**` at that point contributes exactly **1 H**:

```text
H₀ = lines in Codespace/Cosmeron/** at entropy-origin
```

History before the Cosmeron origin is not reconstructed into the new public entropy value. Historical development remains available through the predecessor repository and Git history, while the Cosmeron entropy series begins from its own canonical source state.

## Changes after the origin

For every commit on the canonical first-parent history:

```text
ΔH = additions + deletions
Hₙ = H₀ + ΣΔH
```

Therefore:

```text
added line      = +1 H
deleted line    = +1 H
rewritten line  = +2 H
```

A rewritten line contributes twice because Git represents it as one removed state and one introduced state. A refactor may reduce source size while still increasing Entropy.

Entropy is accumulated commit by commit. Changing a line and later restoring the original text still increases Entropy because the library passed through additional states.

The canonical calculation follows **first-parent history**, so squash-merged pull requests count the state transition that actually entered the main line.

## Greek bands

Each **10,000 H** selects a Greek band:

```text
0–9,999 H          α
10,000–19,999 H    β
20,000–29,999 H    γ
...
230,000–239,999 H  ω
```

After `ω`, symbols continue like spreadsheet columns using a bijective base-24 alphabet:

```text
ω → αα → αβ → αγ → ... → αω → βα → ...
```

The absolute number never resets.

## Reproducibility

The implementation belongs under:

```text
.github/entropy/
```

and must derive the baseline from `entropy-origin`, sum `git diff --numstat` additions and deletions across first-parent history, and keep generated state outside the canonical source scope.

`.gitattributes` fixes the source tree to LF so CRLF/LF conversions cannot artificially inflate Entropy.

Automated tests for the Entropy algorithm should cover the initial snapshot, additions, deletions, rewrites, files outside scope, renames, reverts, Greek-band rollover, and the public display format.

## Release identity

Cosmeron does not use semantic versioning for canonical releases. A release is identified by the Entropy state at which it is published.

See [RELEASES.md](RELEASES.md) for the release log.
