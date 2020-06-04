import platform


_THIS_SYSTEM = platform.system()


def _tokenize_path_envvar_unix(path_envvar):
    tokens = list()
    i = 0
    while i < len(path_envvar):
        curr_char = path_envvar[i] if i < len(path_envvar) else None
        if curr_char == ":":
            tokens.append(curr_char)
            i += 1
        else:
            curr_token = ""
            while curr_char != ":" and i < len(path_envvar):
                curr_token += curr_char
                i += 1
                curr_char = path_envvar[i] if i < len(path_envvar) else None
            tokens.append(curr_token)
    return tokens


def _tokenize_path_envvar_windows(path_envvar):
    tokens = list()
    i = 0
    while i < len(path_envvar):
        curr_char = path_envvar[i] if i < len(path_envvar) else None
        if curr_char == ";":
            tokens.append(curr_char)
            i += 1
        elif curr_char == "\"":
            curr_token = ""
            i += 1  # Skip the first double quote character.
            curr_char = path_envvar[i] if i < len(path_envvar) else None
            while curr_char != "\"" and i < len(path_envvar):
                curr_token += curr_char
                i += 1
                curr_char = path_envvar[i] if i < len(path_envvar) else None
            i += 1  # Skip the second double quote character.
            curr_char = path_envvar[i] if i < len(path_envvar) else None
            tokens.append(curr_token)
        else:
            curr_token = ""
            while curr_char != ";" and i < len(path_envvar):
                curr_token += curr_char
                i += 1
                curr_char = path_envvar[i] if i < len(path_envvar) else None
            tokens.append(curr_token)
    return tokens


def _tokenize_path_envvar(path_envvar):
    global _THIS_SYSTEM

    if _THIS_SYSTEM == "Windows":
        return _tokenize_path_envvar_windows(path_envvar)
    else:
        return _tokenize_path_envvar_unix(path_envvar)


def parse_path_envvar(path_envvar, empty_path=""):
    global _THIS_SYSTEM

    tokens = _tokenize_path_envvar(path_envvar)
    paths = list()
    added_paths = set()
    expected_path = True
    host_path_sep = ";" if _THIS_SYSTEM == "Windows" else ":"
    for token in tokens:
        if token == host_path_sep:
            if expected_path:
                if empty_path not in added_paths:
                    paths.append(empty_path)
                    added_paths.add(empty_path)
            expected_path = True
        else:
            if token not in added_paths:
                paths.append(token)
                added_paths.add(token)
            expected_path = False
    return paths
