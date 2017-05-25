var binding = require('bindings')('binding.node');

var http = require("http");
path = require("path"),
url = require("url"),
filesys = require("fs"),
express = require('express'),
mime = require('mime');

var app = express();

var port = 8080;
var host = '127.0.0.1';

// Set the views directory and jade as the templating engine.
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

// binding.init();

// Start server.
var server = http.createServer(app).listen(port);

if (process.argv.length != 4) {
  console.log("Wrong number of arguments.");
  process.exit();
}

binding.loadDocCollection(process.argv[2]);
binding.loadInvertedIndex(process.argv[3]);

// At root we return a view with the query results.
app.get('/', function (req, res) {
  var query = req.query.q || '';
  var pageRank = req.query.pagerank == "true";
  var anchorRank = req.query.anchor == "true";
  var page = parseInt(req.query.page);

  console.log(req.query);
        
  var arr = binding.search(query);
  console.log(JSON.stringify(arr));
  // var arr = str.split('\n');
  var numResults = arr[0];
  
  res.render('index', {
    numResults: arr.length,
    results: arr,
    page: page,
    q: query,
    time: 0.011,
    pageRank: pageRank,
    anchorRank: anchorRank
  });
});

// At /doc we return the document content.
app.get('/doc', function (req, res) {
  var docId = parseInt(req.query.id);
        
  // res.send(addon.getDocumentText(docId));
});
