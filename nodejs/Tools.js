const fs = require('fs');

const TOOLS = {
    clearElement: function(element){
        element.innerHTML = "";
    },
    readFile: function(path){
        return fs.readFileSync(path).toString().split("\n").join(" ");
    },
    fileExists: function(path){
        return fs.existsSync(path);
    }, 
    getParam: function(name, single = false){
        const args = process.argv;
        for(let i = 0; i < args.length; ++i){
            if(args[i] == name){
                return {
                    name,
                    value: single ? null : (i < args.length-1 ? args[i+1] : null)
                };
            }
        }
        return null;
    },
    cleanNewLine: function(input){
        let result = "";

        for(let i = 0; i < input.length; ++i){
            const c = input.charAt(i);
            if(c == "\n"){
                continue;
            }
            result += c;
        }
        return result;
    },
    cleanSideBrackets: function(input){
        return input.charAt(0) == "[" && input.charAt(input.length-1) == "]" ? input.substr(1, input.length-1) : input+"";
    },
    cleanEmptyEntry: function(input){
        if(input.length == 0){
            return "";
        }
        for(let i = 0; i < input.length; ++i){
            if(input.charAt(i) != " "){
                return input;
            }
        }
        return "";
    },
    removeExtraSpaces: function(input){
        let result = "";
        let lc = "";
        for(let i = 0; i < input.length; ++i){
            const c = input.charAt(i)
            if(c == " " && lc == " "){
                continue;
            }
            result += c;
            lc = c;
        }
        return result;
    },
    removeSpecialChars: function(input){
        const list = [
            "!", "@", "'", "\"","`", "#", "%", "*",
            "&", "^", "[","]", "{", "}", "|", "(",
            ")", "-", "=", "+", ";", ".", ",",
            "/", "\\", "?", "$", "<", ">", "–"
        ];
        let result = input;
        for(let i = 0; i < list.length; ++i){
            // probably kind of slow
            // will do for now
            result = result.replaceAll(list[i], "");
        }
        return result;
    }    
};

module.exports = TOOLS;