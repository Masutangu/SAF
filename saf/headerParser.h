//
// Created by Masutangu on 16/8/25.
//

#ifndef SAF_HEADERPARSER_H
#define SAF_HEADERPARSER_H


namespace saf {

typedef int (*parse_header_func)(char* buf, unsigned int len);

class HeaderParser {

public:
    int parseHeader(char* buf, unsigned int len) {
        return headerParser(buf, len);
    }
    void registerHeaderParse(parse_header_func func) {
        headerParser = func;
    }

private:
    parse_header_func headerParser;
};

#define HEADER_PARSER_INSTANCE (*(Singleton<HeaderParser>::instance()))

#define REGISTER_HEADER_PARSER(headerProcFunc) \
    HEADER_PARSER_INSTANCE.registerHeaderParse(headerProcFunc);
}


#endif //SAF_HEADERPARSER_H
