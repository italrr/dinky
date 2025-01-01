const Jimp = require('jimp');

const BITMAP = {
    build: function(w, h, chan){
        return {
            data: new Jimp.Jimp({ width: w, height: h, color: 0xffffffff }),
            writePNG: async function(filename){
                await this.data.write(filename);
            }

        };
    },
    generate: function(document){

    }
};

module.exports = BITMAP;

