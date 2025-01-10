#include "common/Tools.hpp"
#include "dinky.hpp"


std::shared_ptr<DNK::Document> DNK::build(const std::string &title, const std::string &author, unsigned type){
    auto doc = std::make_shared<DNK::Document>();

    doc->title = title;
    doc->author = author;
    doc->type = type;

    return doc;
}

namespace Parser {
    int findNextFragment(const std::string &input, char find, int from){
        // will ignore things '
        auto inStr = false;
        for(int i = from; i < input.size(); ++i){
            if(input[i] == '\''){
                inStr = !inStr;
            }
            if(inStr){
                continue;
            }
            if(input[i] == find){
                return i;
            }
        }
        return -1;
    }
    std::vector<std::string> splitAroundFragment(const std::string &input, char find){
        // will ignore things '
        std::vector<std::string> output;
        auto inStr = false;
        std::string str = "";
        for(int i = 0; i < input.size(); ++i){
            auto c = input[i];
            if(c == '\''){
                inStr = !inStr;
            }
            if(c == find && !inStr || i == (input.size()-1)){
                if(i == (input.size()-1)){
                    str += c;
                }
                output.push_back(str);
                str = "";
            }else{
                str += c;
            }
        }
        return output;
    }
    std::string parseLiteralParam(const std::string &input){
        std::string result = input;
        if(result[0] == '\'' && result[result.size()-1] == '\''){
            result = input.substr(1, input.size()-2);
        }
        if(result[result.size()-1] !=  '%'){ // TODO: test for number
            result += "rem";
        }
        return result;        
    }
    int findNextBlock(const std::string &input, int from){
        int brackets = 0;
        for(int i = from; i < input.size(); ++i){
            if(input[i] == '[' && ( i == 0 || i > 0 && input[i-1] != '[')){
                ++brackets;
            }
            if(input[i] == ']' && ( i == 0 || i > 0 && input[i-1] != ']')){
                --brackets;
                if(brackets == 0){
                    return i;
                }
            }
        }
        return -1;        
    }

    std::unordered_map<std::string, std::string> fetchMediaParams(const std::string &input){
        std::unordered_map<std::string, std::string> params;
        auto items = Parser::splitAroundFragment(input, ' ');
        for(int i = 0; i < items.size(); ++i){
            auto &item = items[i];
            auto subitems = Parser::splitAroundFragment(item, ':');
            auto id = subitems[0];
            auto value = Parser::parseLiteralParam(subitems[1]);
            params[id] = value;
        }
        return params;
    }

    std::string cleanNewLine(const std::string &input){
        std::string result = "";

        for(int i = 0; i < input.size(); ++i){
            auto c = input[i];
            if(c == '\n'){
                continue;
            }
            result += c;
        }
        return result;        
    }
    
    std::string cleanEmptyEntry(const std::string &input){
        if(input.size() == 0){
            return "";
        }
        for(int i = 0; i < input.size(); ++i){
            if(input[i] != ' '){
                return input;
            }
        }
        return "";
    }

    std::shared_ptr<DNK::Node> read(const std::string &src, std::vector<std::shared_ptr<DNK::Node>> &head){


        auto input = Parser::cleanNewLine(src);

        std::string str = "";
        char lastChar;

        auto node = std::make_shared<DNK::Node>();
        node->type = DNK::NodeType::PANEL;
        node->str = "";

        for(int i = 0; i < input.size(); ++i){
            char c = input[i];
            // Helpers
            if(c == '%'){
                auto closed = Parser::findNextFragment(input, '%', i+1);
                auto media = input.substr(i+1, closed-i-1);
                auto empty = media.find(" ");
                auto token = media.substr(0, empty);
                auto literal = media.substr(empty+1);
                auto params = Parser::fetchMediaParams(literal);
                node->type = DNK::NodeType::type(DNK::String::upper(token));
                node->params = params;
                i = closed;
            }else
            // Styling
            if(c == '!'){
                auto empty = input.find(' ', i);
                auto eq = input.find('=', i);
                if(eq == -1 || eq > empty){
                    auto token = input.substr(i+1, empty - i-1);
                    i = empty;
                    node->styling[token] = ""; 
                }else{
                    auto token = input.substr(i+1, eq - i-1);
                    auto param = input.substr(eq+1, empty - eq-1);
                    i = empty;
                    node->styling[token] = param;
                }
            }else
            // Child
            if(c == '['){
                auto closed = Parser::findNextBlock(input, i);
                auto substr = input.substr(i+1, closed - i-1);
                Parser::read(substr, node->children);
                i = closed;
            }else

            // Simple text
            if(c == ' ' && lastChar == ' '){
                continue;
            }else{
                str += c;
                lastChar = c;
            }
        }
        head.push_back(node);
        node->str = Parser::cleanEmptyEntry(str);
        if(node->str.size() > 0 && node->type == DNK::NodeType::PANEL){
            node->type = DNK::NodeType::TEXT;
        }

        return node;
    }
}

bool DNK::parse(const std::shared_ptr<DNK::Document> &target, const std::string &src){
    std::vector<std::shared_ptr<DNK::Node>> body; // Should be only one
    Parser::read(src, body);
    if(body.size() == 0 || body.size() > 1){
        fprintf(stderr, "Fatal error parsing document: Only one root node is expected for a document, but instead got %i node(s).\n", body.size());
        return false;
    }
    target->body = body[0];
    return true;
}

int main(int argc, char* argv[]){
    auto params = DNK::Core::loadParams(argc, argv);

    auto iFile = DNK::Core::getParam(params, "-f");
    auto iText = DNK::Core::getParam(params, "-i");
    auto iVersion = DNK::Core::getParam(params, "-v", true);
    auto iHelp = DNK::Core::getParam(params, "-h", true);

    if(iHelp.isValid){
        printf("dinky: HELP\n");
        return 0;
    }

    if(iVersion.isValid){
        printf("dinky\n");
        return 0;
    }    

    if(!iFile.isValid && !iText.isValid || (iFile.isValid && iFile.value == "") || (iText.isValid && iText.value == "")){
        fprintf(stderr, "No input was provided.\n");
        return 1;
    }

    if(iFile.isValid && iText.isValid){
        fprintf(stderr, "Two types of input were provided, only one was is required.\n");
        return 1;    
    }

    std::string title = "Untitled";
    std::string author = "Anonymous";
    std::string input = "[%title v:'Dinky!'%] [Dinky is a text]";
    unsigned type = DNK::DocumentType::TYPICAL;

    if(iText.isValid){
        input = iText.value;
    }else
    if(iFile.isValid){
        if(!DNK::File::exists(iFile.value)){
            fprintf(stderr, "File '%s' doesn't exist\n", iFile.value.c_str());
            return 1;
        }else{
            input = DNK::File::readTextFile(iFile.value);
        }
    }

    auto document = DNK::build(title, author, type);
    if(!DNK::parse(document, input)){
        fprintf(stderr, "Failed to parse document\n");        
        return 1;
    }

    if(!DNK::RenderImage(document, "Test.png", 3)){
        fprintf(stderr, "Failed to render document\n");        
        return 1;        
    }

    return 0;
}