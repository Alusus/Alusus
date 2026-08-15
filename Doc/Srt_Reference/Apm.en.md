# Apm Reference

`Apm` (Alusus Package Manager) is used to install and import packages directly from GitHub. It is possible to use this class
from inside the program directly, or use the corresponding `apm` command line tool.

To get usage guideline for Apm from the command line you can write the following command:

```
$ apm help
```

Importing packages using Apm inside a program is done using `importPackage` function:

```
import "Apm.alusus";
Apm.importPackage("&lt;author&gt;/&lt;pkg name&gt;@&lt;version&gt;" [, "&lt;filename&gt;"]);
Apm.importPackage("&lt;author&gt;/&lt;pkg name&gt;@&lt;version&gt;", { "&lt;filename1&gt;", "&lt;filename2&gt;", ... });
```

The version must be specified and can take one of the following forms:
* **&lt;major&gt;:** Matches the latest release with the given major version number.
* **&lt;major&gt;.&lt;minor&gt;:** Matches the latest release with the given major and minor version numbers.
* **&lt;major&gt;.&lt;minor&gt;.&lt;patch&gt;:** Exact version match.

The second form of the function allows the user to include multiple files from the package at once.
Using the second form to include multiple files is faster than calling the first form multiple times, because each call to this function will
cause reading information from GitHub.

While developing packages usually the developer needs to try the package locally before pushing the changes to GitHub.
Users can use the link command to link a certain package with a local folder, then Apm will use the local copy instead of
downloading the GitHub copy.

Link command must be executed inside the project directory that uses the package and has the following form:

```
$ cd &lt;example_project_folder&gt;
$ apm link &lt;author&gt;/&lt;package_name&gt;@&lt;release&gt; &lt;path_to_local_package_copy&gt;
```

It is possible to remove the link using `unlink` command, which has the following form:

```
$ cd &lt;example_project_folder&gt;
$ apm unlink &lt;author&gt;/&lt;package_name&gt;@&lt;release&gt;
```

The following program options can be passed to Alusus at runtime to control package fetching behavior:
* **apm_check_updates:** Always fetch the list of available releases from GitHub to determine
the most recent version matching the requested version spec, and download it if not already available locally. Without this
option, APM uses the first locally cached version it finds without checking for newer releases.

* **apm_refetch:** Always download the latest matching package from GitHub, ignoring any
locally cached copies.
