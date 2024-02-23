import { exec } from 'child_process';
import express from 'express';
import path from 'path';
import { fileURLToPath } from 'url';
import fs from 'fs';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename); 

const app = express();
const port = 3000;

app.use(express.static(__dirname + '/public'));

app.use('/libs', express.static(path.join(__dirname, '..', 'node_modules')));

app.listen(port, () => {
  console.log(`Listening at http://localhost:${port}`);
});

const cfg = './build/cfg';

const options = ['input.cpp'];

app.get('/run-command', (req, res) => {
  exec('./build/cfg input.cpp', (error, stdout, stderr) => {
      if (error) {
          console.error(`exec error: ${error}`);
          return res.status(500).send(`exec error: ${error}`);
      }
      fs.readFile(__dirname + '/../cfg.dot', 'utf8', (err, data) => {
        if (err) {
            console.error(err);
            return;
        } else {
          res.send({ data, stderr });
        }
    });
      // res.send({ stdout, stderr });
  });
});



