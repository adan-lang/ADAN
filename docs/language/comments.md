## Documenting Code

Writing comments in ADAN is very similar to beginner-friendly languages, such as Python.

Single-lined comments can be written with the `#` syntax. Unlike Python, multi-lined comments can be written using `##`, as seen in the example below:

```adan
# This is a single-lined comment.

##
This is a multi-lined comment,
  anything before the final ## will be
  excluded at compilation.

That means you can have as MANY #'s as you'd like!

- The only ones that matter are the very first ones, and the very last ones :)
##

This will error! :(
```