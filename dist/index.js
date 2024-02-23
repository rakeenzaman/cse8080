"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const child_process_1 = require("child_process");
const cfg = './build/cfg';
const options = ['input.cpp'];
const child = (0, child_process_1.spawn)(cfg, options);
child.stdout.on('data', (data) => {
    console.log(`stdout: ${data}`);
});
child.stderr.on('data', (data) => {
    console.error(`stderr: ${data}`);
});
child.on('close', (code) => {
    console.log(`Child process exited with code ${code}`);
});
console.log("yeah");
//# sourceMappingURL=index.js.map