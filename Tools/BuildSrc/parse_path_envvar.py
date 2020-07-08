import platform


_THIS_SYSTEM = platform.system()


def _tokenize_path_envvar(path_envvar):
    global _THIS_SYSTEM

    host_path_sep = ";" if _THIS_SYSTEM == "Windows" else ":"
    tokens = list()
    i = 0
    while i < len(path_envvar):
        curr_char = path_envvar[i]
        if curr_char == host_path_sep:
            tokens.append({
                "is_sep": True,
                "text": curr_char
            })
            i += 1
        elif curr_char == "\"" and _THIS_SYSTEM == "Windows":
            curr_token = ""
            i += 1  # Skip the first double quote character.
            if i < len(path_envvar):
                curr_char = path_envvar[i]
                while curr_char != "\"" and i < len(path_envvar):
                    curr_token += curr_char
                    i += 1
                    if i < len(path_envvar):
                        curr_char = path_envvar[i]
                i += 1  # Skip the second double quote character.
                tokens.append({
                    "is_sep": False,
                    "text": curr_token
                })
        else:
            curr_token = ""
            while curr_char != host_path_sep and i < len(path_envvar):
                curr_token += curr_char
                i += 1
                if i < len(path_envvar):
                    curr_char = path_envvar[i]
            tokens.append({
                "is_sep": False,
                "text": curr_token
            })
    return tokens


def parse_path_envvar(path_envvar, empty_path=""):
    tokens = _tokenize_path_envvar(path_envvar)
    paths = list()
    added_paths = set()
    expected_path = True
    for token in tokens:
        if token["is_sep"]:
            if expected_path:
                if empty_path not in added_paths:
                    paths.append(empty_path)
                    added_paths.add(empty_path)
            expected_path = True
        else:
            if token["text"] not in added_paths:
                paths.append(token["text"])
                added_paths.add(token["text"])
            expected_path = False
    return paths
