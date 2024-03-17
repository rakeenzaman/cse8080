string escapeDoubleQuotes(const string& input) {
    string output;

    for (char ch : input) {
        if (ch == '\"') {
            output += "\\\"";
        } else {
            output += ch;
        }
    }
    return output;
}