#ifndef DNK_DINKY_HPP
    #define DNK_DINKY_HPP

    #include <unordered_map>
    #include "common/Types.hpp"
    #include "common/Tools.hpp"

    namespace DNK {

        namespace DocumentType {
            enum DocumentType : unsigned {
                TYPICAL
            };
        }

        namespace SettingsType {
            enum SettingsType : unsigned {
                SCALABLE,
                FIXED
            };
        }        

        namespace NodeType {
            enum NodeType : unsigned {
                PANEL,
                TEXT,
                SUBTITLE,
                TITLE,
                CARDVIDEO
            };

            static int type(const std::string &name){
                if(name == "PANEL"){
                    return NodeType::PANEL;
                }else
                if(name == "TEXT"){
                    return NodeType::TEXT;
                }else
                if(name == "SUBTITLE"){
                    return NodeType::SUBTITLE;
                }else
                if(name == "TITLE"){
                    return NodeType::TITLE;
                }else
                if(name == "CARD-VIDEO"){
                    return NodeType::CARDVIDEO;
                }else{
                    return NodeType::PANEL;                    
                }       
            }

            static std::string name(int type){
                switch(type){
                    default:
                    case NodeType::PANEL: {
                        return "PANEL";
                    };
                    case NodeType::TEXT: {
                        return "TEXT";
                    };
                    case NodeType::SUBTITLE: {
                        return "SUBTITLE";
                    };
                    case NodeType::TITLE: {
                        return "TITLE";
                    };               
                    case NodeType::CARDVIDEO: {
                        return "CARD-VIDEO";
                    };                         
                }
            }
        }

        struct Node {
            unsigned type;
            std::unordered_map<std::string, std::string> styling;
            std::string str;
            std::unordered_map<std::string, std::string> params;
            std::vector<std::shared_ptr<DNK::Node>> children;

            Node(){
            }

            float getNumberStyle(const std::string &name){
                if(styling.count(name) == 0 || !DNK::String::isNumber(styling[name])){
                    return 0.0f;
                }
                return DNK::String::number(styling[name]);
            }

            bool getBoolStyle(const std::string &name){
                if(styling.count(name) == 0){
                    return false;
                }
                return DNK::String::lower(styling[name]) == "true";
            }

            std::string getStringStyle(const std::string &name){
                if(styling.count(name) == 0){
                    return "";
                }
                return styling[name];
            }

        };

        struct Settings {
            unsigned type;
            std::string width;
            std::string height;
            std::string colorspace;
            float aratio; // w/h
            Settings(){
                this->type = DNK::SettingsType::SCALABLE;
                width = "0";
                height = "0";
                colorspace = "rgb";
                aratio = 0.772727273;
            }
        };

        struct Document {
            std::string title;
            std::string author;
            unsigned type;
            uint64 createdAt;
            uint64 modifiedAt;
            std::shared_ptr<DNK::Node> body;
            DNK::Settings settings;
        };

        // Parse/Read
        std::shared_ptr<DNK::Document> build(const std::string &title, const std::string &author, unsigned type);
        bool parse(const std::shared_ptr<DNK::Document> &target, const std::string &src);

        // Render
        bool RenderImage(const std::shared_ptr<DNK::Document> &doc, const std::string &filename, unsigned pd);
    }

#endif