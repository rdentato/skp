# 
<p width="100%" style="align:center">
<img height="150" src="https://raw.githubusercontent.com/rdentato/skp/master/docs/oval_logo_small.png"> <br/>
 
</p>

Skp is a library of functions to help parsing text files.

It offers three *levels*:
  - **Skipping**, where the focus is to identify portion of text that can be skipped according a pattern;
  - **Scanning**, where a text is repeatedly checked against a set of patterns;
  - **Parsing**, where a Parsing Expression Grammar can be directly coded as a recursive descent parser.

Skp assumes that the text is UTF-8 encoded, ISO-8859-1 encoding can also be used.
Being the focus on parsing programming languages and data files, the character classes
(uppercase/lower, digit, ...) are limited to the ASCII range.
Other clasess can be specified explicitly. For example you can match an hiragana character
with `[ぁ-ゖ]`.

For further information have a look at the [documentation](https://rdentato.github.io/skp/) or chat with us on <a href="https://discord.gg/vPgsxHcgXX"><img src="https://github.com/rdentato/skp/blob/master/docs/Discord_button.jpg?raw=true" title="Discord Channel"></a>. 
