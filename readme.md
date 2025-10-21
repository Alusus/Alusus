<img
  alt="ALUSUS PROGRAMMING LANGUAGE"
  align="right"
  src="https://alusus.org/Resources/logo.gif"
/>

# ALUSUS PROGRAMMING LANGUAGE [[عربي]](readme.ar.md)

Alusus is designed to be a language for everything by making language features hot-plugable, and on a per-project basis
 (rather than per-environment), which allows the language to:
* Expand vertically to cover low level features like those in C++ as well as high level features like those found in
languages like Python or Ruby.
* Expand horizontally to cover all aspects of programming which saves the user having to use more than one language.
* Allow anyone to enhance the language in a decentralized way, which allows users to develop new programming techniques
or paradigms without having to start from scratch with a new language.

Imagine being able to write your client side code, server side code, DB access code, GPU shaders, etc, all in one
language. Imagine switching between programming paradigms becomes a matter of picking up a different library rather than
a different language. Imagine writing your program in a high level language while still being able to switch to low
level for performance critical components. The goal of Alusus is to eventually make all that possible, but development
is still in an early stage, and as of now Alusus provide low level procedural programming features.

Visit [alusus.org](https://alusus.org) for more info.

## Downloads

Visit the [downloads](https://alusus.org/download.html) page for pre-built deb and rpm packages, or pre-built zip package.

## Installation

Download the package for your system from the downloads page. Or just unpack the zip file into any directory on your
system.

For Windows, you'll need to first enable WSL then either install the Linux package appropriate for your distro, or
unpack the zip package.

## Documentation

Documentation is still lacking, but you can find some high level design documentation and examples on the
[documentation](https://alusus.org/documentation.html) page.

## Directory Structure

* `/Doc`: Contains all the documentation of the project, including the language specifications, implementation
  documentation, and source code inline documentation.

* `/Sources`: Contains the entire source code.
  - `/Sources/Core`: Contains the source code of the Core.
  - `/Sources/Spp`: Contains the source code of the Standard Programming Paradigm library.
  - `/Sources/Srt`: Contains the source for the Standard Runtime Library.
  - `/Sources/Tests`: Contains automated test projects for Core and the standard libraries.

* `/Notices_L18n`: Contains localizations for build notices.

* `/Examples`: Contains examples written in Alusus.

* `/Tools`: Contains various development helper scripts.

## Help Us

Our team needs volunteers to contribute to this project. Join us in making the future better for programmers. Visit the
[development](https://alusus.org/development.html) page for more information.

Or help by donating to the project<br/>
[![PayPal - The safer, easier way to pay online!](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://paypal.me/alusus)

## Collaborators

* [sarmadka](https://github.com/sarmadka) -
**Sarmad Khalid Abdullah** &lt;sarmad@alusus.org&gt; *Inventor and lead engineer*
* [falhumai96](https://github.com/falhumai96) -
**Faisal Al-Humaimidi** &lt;falhumai96@gmail.com&gt; *Developer*
* [xlmnxp](https://github.com/xlmnxp) -
**Salem Yaslem Saleh** &lt;s@sy.sa&gt; *Developer*
* [rafidka](https://github.com/rafidka) -
**Rafid K. Al-Humaimidi** &lt;rafidka@gmail.com&gt; *Developer*
* **Hicham OUALI ALAMI** *Developer*

## Copyright

Copyright (C) 2025.
The copyright for this project is owned by Sarmad Khalid Abdullah.

## [LICENSE](license.txt)

Alusus Language specification, documentation, source code, binaries, and examples are published under Alusus Public
License, Version 1.0, which is included in this repo and is also available online at
[https://alusus.org/license.html](https://alusus.org/license.html). Please read the license before using
or copying this software. By using this software you acknowledge that you have read the terms contained in this license
and agree with and accept all such terms.

Alusus Public License is designed to make the language open source while protecting the language from fragmenting into
multiple incompatible languages at an early stage. The license allows:
* Obtaining the language in source code or binary format free of charge and use it to write any program, whether
  commercial or not.
* Modify the language and use the modified version to write any program, whether commercial or not.
* Redistribute the unmodified version of the language in source code or binary format. The license prevents distributing
  a modified version of the language except with a permission from Alusus Software Ltd.
