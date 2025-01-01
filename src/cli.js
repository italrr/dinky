const Tools = require('./Tools');
const DINKY = require('./dinky');
const BITMAP = require('./renderer/bitmap');

const iFile = Tools.getParam("-f");
const iText = Tools.getParam("-i");
const iVersion = Tools.getParam("-v", true);
const iHelp = Tools.getParam("-h", true);

if(!iFile && !iText || (iFile && !iFile.value) || (iText && !iText.value)){
    console.error("No input was provided.");
    return 1;
}

if(iFile && iText){
    console.error("Two types of input were provided, only one was is required.");
    return 1;    
}

if(iHelp){
    console.log("dinky: HELP");
    return 0;
}

if(iVersion){
    console.log("dinky");
    return 0;
}

let input = `
    [%title v:'Dinky!'%]
    [Dinky is a text]
`;

if(iText){
    input = iText.value;
}else
if(iFile){
    if(!Tools.fileExists(iFile.value)){
        console.error(`File "${iFile.value}" doesn't exist.`);
        return 1;
    }else{
        input = Tools.readFile(iFile.value);
    }
}

const document = DINKY.generate();
DINKY.read(input, document.children);
//console.log(JSON.stringify(document, null, 4))



const img = BITMAP.build(256, 256, 4);
img.writePNG("XD.png");
