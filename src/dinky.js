const Tools = require('./Tools');

const DINKY = {
    generate: function(){
        return {
            settings: {
                type: "scalable",
                width: "1",
                height: "1",
                colorspace: "rgb",
                ppi: "1"
            },
            type: "TYPICAL", // Top down, left to right
            children: [],
            title: null,
            createdAt: null,
            modifiedAt: null,
            author: "undefined"
        };
    },
    findNextFragment: function(input, find, from){
        // will ignore things '
        let inStr = false;
        for(let i = from; i < input.length; ++i){
            if(input.charAt(i) == "'"){
                inStr = !inStr;
            }
            if(inStr){
                continue;
            }
            if(input.charAt(i) == find){
                return i;
            }
        }
        return -1;
    },  
    fetchMediaParams: function(input){
        const params = {};
        const items = DINKY.splitAroundFragmet(input, ' ');
        for(let i = 0; i < items.length; ++i){
            const item = items[i];
            const subitems = DINKY.splitAroundFragmet(item, ":");
            const id = subitems[0];
            const value = DINKY.parseLiteralParam(subitems[1]);
            params[id] = value;
        }
        return params;
    },   
    splitAroundFragmet: function(input, find){
        // will ignore things '
        const output = [];
        let inStr = false;
        let str = "";
        for(let i = 0; i < input.length; ++i){
            const c = input.charAt(i);
            if(c == "'"){
                inStr = !inStr;
            }
            if(c == find && !inStr || i == (input.length-1)){
                if(i == (input.length-1)){
                    str += c;
                }
                output.push(str);
                str = "";
            }else{
                str += c;
            }
        }
        return output;
    }, 
    parseLiteralParam: function(input){
        let result = input;
        if(result.charAt(0) == "'" && result.charAt(result.length-1) == "'"){
            result = input.substring(1, input.length-1);
        }
        if(result.charAt(result.length-1) !=  "%" && !isNaN(result)){
            result += "rem";
        }
        return result;
    },        
    findNextBlock: function(input, from){
        let brackets = 0;
        for(let i = from; i < input.length; ++i){
            if(input.charAt(i) == "[" && ( i == 0 || i > 0 && input.charAt(i-1) != "\[")){
                ++brackets;
            }
            if(input.charAt(i) == "]" && ( i == 0 || i > 0 && input.charAt(i-1) != "\]")){
                --brackets;
                if(brackets == 0){
                    return i;
                }
            }
        }
        return -1;
    },        
    read: function(src, head){
        const input = Tools.cleanSideBrackets(Tools.cleanNewLine(src));

        let str = "";
        let lastChar = "";

        const block = {
            type: "TEXT",
            params: null,
            styling: {},
            str: "",
            children: []
        };

        for(let i = 0; i < input.length; ++i){
            const c = input.charAt(i);
            // Helpers
            if(c == "%"){
                const closed = DINKY.findNextFragment(input, "%", i+1);
                const media = input.substring(i+1, closed);
                const empty = media.indexOf(" ");
                const token = media.substring(0, empty);
                const params = DINKY.fetchMediaParams(media.substring(empty+1));
                block.type = token.toUpperCase();
                block.params = params;
                i = closed;
            }else
            // Styling
            if(c == "!"){
                const empty = input.indexOf(' ', i);
                const eq = input.indexOf('=', i);
                if(eq == -1 || eq > empty){
                    const token = input.substring(i+1, empty);
                    i = empty;
                    block.styling[token] = null;
                }else{
                    const token = input.substring(i+1, eq);
                    const param = input.substring(eq+1, empty);
                    i = empty;
                    block.styling[token] = param;
                }
            }else
            // Child
            if(c == "["){
                const closed = DINKY.findNextBlock(input, i);
                const substr = input.substring(i+1, closed);
                DINKY.read(substr, block.children);
                i = closed+1;
            }else
            // Simple text
            if(c == " " && lastChar == " "){
                continue;
            }else{
                str += c;
                lastChar = c;
            }
        }
        head.push(block);
        console.log(str)
        block.str = Tools.cleanEmptyEntry(str);
    }
};

module.exports = DINKY;