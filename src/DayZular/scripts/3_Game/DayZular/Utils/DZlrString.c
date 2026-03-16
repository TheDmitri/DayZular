// DayZular Framework - String Utilities
// Standalone string helper class (CF_String alternative)

typedef string DZlrString;
class DZlrString : string
{
    /**
     * @brief Returns the number of times the specified character exists in the string
     * @param char The character that will be matched
     */
    int CountCharacter(DZlrString char)
    {
        int count = 0;
        int length = value.Length();
        int index = 0;
        while (index < length)
        {
            count += value.Substring(index, 1) == char;
            index++;
        }
        return count;
    }

    /**
     * @brief Returns the number of times the specified character exists in the string
     * @param char The character that will be matched
     * @param[out] firstOccurrence The index of the first character
     */
    int CountCharacter(DZlrString char, out int firstOccurrence)
    {
        int count = 0;
        firstOccurrence = -1;

        int length = value.Length();
        int index = 0;
        while (index < length)
        {
            count += value.Substring(index, 1) == char;

            if (count == 1 && firstOccurrence == -1)
            {
                firstOccurrence = index;
            }
            index++;
        }

        return count;
    }

    /**
     * @brief Finds last 'sample' in 'str'. Returns -1 when not found
     * @param sample Finding string
     * @return Returns position where sample starts, or -1 when sample not found
     */
    int LastIndexOf(DZlrString sample)
    {
        int sampleLength = sample.Length();
        int length = value.Length();
        int start = length - sampleLength;
        if (start <= 0)
        {
            return -1;
        }

        for (int index = start - 1; index >= 0; index--)
        {
            if (value.Substring(index, sampleLength) == sample)
            {
                return index;
            }
        }

        return -1;
    }

    /**
     * @brief Fills the string with the specified character at the start so the string matches the length
     * @param length The total string length 
     * @param padChar The character that will be padded
     */
    string PadStringFront(int length, DZlrString padChar)
    {
        string newString = value;
        length = newString.Length() - length;

        if (length > 0)
        {
            int index = 0;
            while (index < length)
            {
                newString = padChar + newString;
                index++;
            }
        }

        return newString;
    }

    /**
     * @brief Fills the string with the specified character at the end so the string matches the length
     * @param length The total string length 
     * @param padChar The character that will be padded
     */
    string PadStringBack(int length, DZlrString padChar)
    {
        string newString = value;
        length = newString.Length() - length;

        if (length > 0)
        {
            int index = 0;
            while (index < length)
            {
                newString += padChar;
                index++;
            }
        }

        return newString;
    }

    /**
     * @brief Replaces the string of length at the index with the splice
     * @param start The index at which to start changing the string
     * @param splice The string the content will be replaced with
     */
    string SpliceString(int start, DZlrString splice)
    {
        int middle = start + splice.Length();
        int end = value.Length();
        string a = value.Substring(0, start);
        string b = splice;
        if (middle >= end)
        {
            return a + b;
        }

        string c = value.Substring(middle, end - middle);
        return a + b + c;
    }

    /**
     * @brief Replaces the string of length at the index with the splice
     * @param start The index at which to start changing the string
     * @param length The length of characters to be removed
     * @param splice The string the content will be replaced with
     */
    string SpliceString(int start, int length, DZlrString splice)
    {
        int middle = start + length;
        int end = value.Length();
        string a = value.Substring(0, start);
        string b = splice;
        if (middle >= end)
        {
            return a + b;
        }

        string c = value.Substring(middle, end - middle);
        return a + b + c;
    }

    /**
     * @brief Checks to see if the two strings match
     * @param a The first string to be checked
     * @param b The second string to be checked
     */
    static bool Equals(string a, string b)
    {
        return a == b;
    }

    /**
     * @brief Checks to see if the two strings match, ignoring their case
     * @param a The first string to be checked
     * @param b The second string to be checked
     */
    static bool EqualsIgnoreCase(string a, string b)
    {
        a.ToLower();
        b.ToLower();

        return a == b;
    }

    /**
     * @brief Reverses the string
     */
    string Reverse()
    {
        string result;

        int index = value.Length() - 1;
        while (index >= 0)
        {
            result += value.Substring(index, 1);

            index--;
        }

        return result;
    }

    /**
     * @brief Checks if string starts with the specified prefix
     * @param prefix The prefix to check
     */
    static bool StartsWith(string str, string prefix)
    {
        if (!str || !prefix)
            return false;
        
        int prefixLen = prefix.Length();
        if (prefixLen > str.Length())
            return false;
        
        return str.Substring(0, prefixLen) == prefix;
    }

    /**
     * @brief Checks if string ends with the specified suffix
     * @param suffix The suffix to check
     */
    static bool EndsWith(string str, string suffix)
    {
        if (!str || !suffix)
            return false;
        
        int suffixLen = suffix.Length();
        int strLen = str.Length();
        if (suffixLen > strLen)
            return false;
        
        return str.Substring(strLen - suffixLen, suffixLen) == suffix;
    }

    /**
     * @brief Splits a string by delimiter
     * @param str The string to split
     * @param delimiter The delimiter character
     * @param result The output array of strings
     */
    static void Split(string str, string delimiter, out array<string> result)
    {
        result = new array<string>();
        
        if (!str || str.Length() == 0)
            return;
        
        if (!delimiter || delimiter.Length() == 0)
        {
            result.Insert(str);
            return;
        }
        
        int delimLen = delimiter.Length();
        int startPos = 0;
        int foundPos = str.IndexOf(delimiter);
        
        while (foundPos >= 0)
        {
            string part = str.Substring(startPos, foundPos - startPos);
            result.Insert(part);
            
            startPos = foundPos + delimLen;
            foundPos = str.IndexOfFrom(startPos, delimiter);
        }
        
        // Add remaining part
        if (startPos < str.Length())
        {
            string lastPart = str.Substring(startPos, str.Length() - startPos);
            result.Insert(lastPart);
        }
    }

    /**
     * @brief Trims whitespace from both ends of string
     * @param str The string to trim
     */
    static string Trim(string str)
    {
        if (!str || str.Length() == 0)
            return str;
        
        int start = 0;
        int end = str.Length() - 1;
        
        // Trim start
        while (start <= end && (str.Get(start) == " " || str.Get(start) == "\t" || str.Get(start) == "\n" || str.Get(start) == "\r"))
        {
            start++;
        }
        
        // Trim end
        while (end >= start && (str.Get(end) == " " || str.Get(end) == "\t" || str.Get(end) == "\n" || str.Get(end) == "\r"))
        {
            end--;
        }
        
        if (start > end)
            return "";
        
        return str.Substring(start, end - start + 1);
    }

    /**
     * @brief Checks if string is null or empty
     * @param str The string to check
     */
    static bool IsNullOrEmpty(string str)
    {
        return !str || str.Length() == 0;
    }

    /**
     * @brief Checks if string is null, empty, or whitespace
     * @param str The string to check
     */
    static bool IsNullOrWhitespace(string str)
    {
        if (!str || str.Length() == 0)
            return true;
        
        return Trim(str).Length() == 0;
    }
}
