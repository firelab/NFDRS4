//-----------------------------------------------------------------------
// Copyright 2019 Ciaran McHale.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions.
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.  
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//----------------------------------------------------------------------

//--------
// #include's
//--------
#include "AsciiPatternMatch.h"


namespace CONFIG4CPP_NAMESPACE {



bool
asciiPatternMatchInternal(
	const char *			str,
	int						strIndex,
	int						strLen,
	const char *			pattern,
	int						patternIndex,
	int						patternLen)
{
	while (patternIndex < patternLen) {
		if (pattern[patternIndex] != '*') {
			if (pattern[patternIndex] != str[strIndex]) {
				return false;
			}
			patternIndex++;
			if (strIndex < strLen) {
				strIndex++;
			}
		} else {
			patternIndex++;
			while (pattern[patternIndex] == '*') {
				patternIndex++;
			}
			if (patternIndex == patternLen) {
				return true;
			}
			for (; strIndex < strLen; strIndex++) {
				if (asciiPatternMatchInternal(str, strIndex, strLen,
											pattern, patternIndex, patternLen))
				{
					return true;
				}
			}
		}
	}
	if (pattern[patternIndex] != str[strIndex]) {
		return false;
	}
	return true;
}



}; // namespace CONFIG4CPP_NAMESPACE
