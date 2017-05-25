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

var query;

// At root we return a view with the query results.
app.get('/', function (req, res) {
  var vectorRank = req.query.vector || 1;
  var anchorRank = req.query.anchor || 0;
  var pageRank = req.query.pagerank || 0;
  var page = parseInt(req.query.page) || 1;

  var arr = [];
  var numResults = 0;

  var start = new Date();

  if (query != req.query.q && req.query.q) {
    console.log("Query: " + req.query.q);
    binding.search(req.query.q, vectorRank, anchorRank, pageRank);
    query = req.query.q;
  } 

  if (!req.query.q) query = "";

  if (query) {
    arr = binding.getPage(page, 10);
    numResults = binding.getNumResults();
  }

  var duration = new Date() - start;

  res.render('index', {
    numResults: numResults,
    results: arr,
    page: page,
    q: query,
    time: duration / 1000,
    vectorRankW: vectorRank,
    anchorRankW: anchorRank,
    pageRankW: pageRank,
  });
});

// At /doc we return the document content.
app.get('/doc', function (req, res) {
  var docId = parseInt(req.query.id);
  res.send(binding.getDocumentText(docId));
});
