const Jimp = require('jimp');

const BITMAP = {
    generate: async function(output, doc, pageSize = { width: 841, height: 1189 }){
        const canvas = new Jimp.Jimp({ width: pageSize.width, height: pageSize.height, color: 0xffffffff })


        


        await canvas.write(output);        
    }
};

module.exports = BITMAP;

