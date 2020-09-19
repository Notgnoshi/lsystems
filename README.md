# Lindenmayer Systems

A (re)exploration of Lindenmayer Systems.

- [Lindenmayer Systems](#lindenmayer-systems)
- [L-System Rule Parsing](#l-system-rule-parsing)
  - [Basic Usage](#basic-usage)
  - [Stochastic Grammars](#stochastic-grammars)
  - [Context Sensitive Grammars](#context-sensitive-grammars)
  - [All of the Above](#all-of-the-above)
  - [Why not Parametric?](#why-not-parametric)
- [L-String Interpretation](#l-string-interpretation)
- [Line Simplification and Joining](#line-simplification-and-joining)
- [3D Perspective Tweaking](#3d-perspective-tweaking)
- [SVG Generation](#svg-generation)
- [SVG Transformations](#svg-transformations)
- [Pen Plotting](#pen-plotting)

I worked on a [class project](https://github.com/macattackftw/fractal_trees) to implement 3D context-free Lindenmayer systems in graduate school.
This is an attempt on doing the same, but for additional grammar types, and with the intent to build a modular transformation pipeline, with the last stage being the generation of an SVG (even for 3D fractals) suitable for for drawing with my [AxiDraw](https://axidraw.com/)

# L-System Rule Parsing

In [*The Algorithmic Beauty of Plants*](http://algorithmicbotany.org/papers/#abop), Lindenmayer and Prusinkiewicz outlined several types of grammars that could be interpreted as algorithmic models of plans.
These grammars are

1. Context-free grammars
2. Stochastic grammars
3. Context-sensitive grammars
4. Parametric grammars

This project implements a [parser](tools/parse.py) for the first three kinds of grammars.

## Basic Usage

Tokens can be multiple characters, and therefore must be comma-separated.
The rules and axiom are white-space insensitive.

**TODO:** Support whitespace-separated tokens.

```shell
$ tools/parse.py --rule 'a -> a, b' --rule 'b -> a' --axiom=a --iterations=3
abaab
$ tools/parse.py --rule 'a -> ab' --rule 'b -> a' --axiom=a --iterations=30
ab
$ tools/parse.py --rule 'a -> ab' --rule 'ab -> a, a' --axiom=a --iterations=3
abab
```

## Stochastic Grammars

If more than one production rule is given for a single token, the first rule given will be chosen.

```shell
$ tools/parse.py --rule 'a -> a' --rule 'a -> b' --axiom='a,a' --iterations=100
aa
```

Probabilities can be specified like so:

```shell
$ tools/parse.py --rule 'a : 0.5 -> a' --rule 'a : 0.5 -> b' --axiom='a,a' --iterations=1 --log-level INFO
2020-08-30 11:36:24,129 - lsystem.grammar - INFO - Using random seed: 4162256033
aa
$ tools/parse.py --rule 'a : 0.5 -> a' --rule 'a : 0.5 -> b' --axiom='a,a' --iterations=1 --log-level INFO
2020-08-30 11:36:26,368 - lsystem.grammar - INFO - Using random seed: 635680691
ba
$ tools/parse.py --rule 'a : 0.5 -> a' --rule 'a : 0.5 -> b' --axiom='a,a' --iterations=1 --log-level INFO
2020-08-30 11:36:28,439 - lsystem.grammar - INFO - Using random seed: 2707414783
bb
```

A random seed may be given via `--seed`.

## Context Sensitive Grammars

One token of left or right (or both) context may be specified.

```shell
$ tools/parse.py --rule 'a>b -> c' --axiom='a,b' --iterations=1
cb
$ tools/parse.py --rule 'b<a -> c' --axiom='b,a' --iterations=1
bc
$ tools/parse.py --rule 'b<a>b -> c' --axiom='b,a,b' --iterations=1
bcb
```

Note that tokens without any matching rules are simply passed-through.

You can also specify a list of tokens to ignore when considering context.

```shell
$ tools/parse.py --rule 'b<a>b -> c' --rule='#ignore:a' --axiom='b,a,a,b' --iterations=1
bccb
```

## All of the Above

See `tools/parse.py --help`. You can mix and match stochastic, context-sensitive, and context-free rules.
It's also possible to pass a JSON config file to avoid incredibly long and hard-to-remember commandline invocations.

However, be aware I've made no attempt at reconcilling any cases where rules don't make sense.
If rules are poorly-formatted, expect an exception.
If probabilities don't sum to 1, expect an exeption.

## Why not Parametric?

Because I'm writing this for fun.

# L-String Interpretation

`tools/interpret.py` reads L-Strings generated by `tools/parse.py` from `stdin` and writes WKT `LINESTRING Z` output to `stdout`.

```shell
$ tools/parse.py --config examples/sierpinski-tree.json > tree.lstring
$ tools/interpret.py < tree.lstring > tree.wkt
$ tail tree.wkt
LINESTRING Z (0 -13.43502884254441 37.43502884254438, 0 -14.14213562373096 38.14213562373092, 0 -14.8492424049175 38.84924240491747, 0 -14.8492424049175 38.84924240491747, 0 -14.8492424049175 38.84924240491747, 0 -14.8492424049175 39.84924240491747)
LINESTRING Z (0 -15.55634918610405 39.55634918610401, 0 -15.8492424049175 38.84924240491747)
LINESTRING Z (0 -13.72792206135786 36.72792206135783, 0 -14.72792206135786 36.72792206135783, 0 -15.72792206135786 36.72792206135783, 0 -15.72792206135786 36.72792206135783, 0 -15.72792206135786 36.72792206135783, 0 -16.43502884254441 37.43502884254438)
LINESTRING Z (0 -16.72792206135786 36.72792206135783, 0 -16.43502884254441 36.02081528017128)
LINESTRING Z (0 -9.485281374238573 32.48528137423855, 0 -10.48528137423857 32.48528137423855, 0 -11.48528137423857 32.48528137423855, 0 -12.48528137423857 32.48528137423855, 0 -13.48528137423857 32.48528137423855, 0 -14.48528137423857 32.48528137423855, 0 -14.48528137423857 32.48528137423855, 0 -14.48528137423857 32.48528137423855, 0 -15.19238815542512 33.1923881554251, 0 -15.89949493661167 33.89949493661165, 0 -16.60660171779822 34.60660171779819, 0 -16.60660171779822 34.60660171779819, 0 -16.60660171779822 34.60660171779819, 0 -16.60660171779822 35.60660171779819)
LINESTRING Z (0 -17.31370849898476 35.31370849898474, 0 -17.60660171779822 34.60660171779819)
LINESTRING Z (0 -15.48528137423857 32.48528137423855, 0 -16.48528137423857 32.48528137423855, 0 -17.48528137423857 32.48528137423855, 0 -17.48528137423857 32.48528137423855, 0 -17.48528137423857 32.48528137423855, 0 -18.19238815542512 33.1923881554251)
LINESTRING Z (0 -18.48528137423857 32.48528137423855, 0 -18.19238815542512 31.77817459305201)
LINESTRING Z (0 -15.19238815542512 31.77817459305201, 0 -15.89949493661167 31.07106781186546, 0 -16.60660171779822 30.36396103067892, 0 -16.60660171779822 30.36396103067892, 0 -16.60660171779822 30.36396103067892, 0 -17.60660171779822 30.36396103067892)
LINESTRING Z (0 -17.31370849898476 29.65685424949237, 0 -16.60660171779822 29.36396103067892)
```

There are fewer options than `tools/parse.py`, but you can at least configure the stepsize and angle used by the turtle.
See `tools/interpret.py --help` for more information on how the L-Strings are interpreted.

# Line Simplification and Joining

**TODO:** Turn a collection of potentially overlapping (and containing duplicates) line segments, and turn it into a simplified collection of polygonal lines.

# 3D Perspective Tweaking

**TODO:** View the collection of segments (or polygonal lines) in a 3D viewer that allows for modifying the camera perspective.

# SVG Generation

**TODO:** Simple for the 2D case, but 3D isn't so trivial.

# SVG Transformations

**TODO:**

# Pen Plotting

**TODO:**
