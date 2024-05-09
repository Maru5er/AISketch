console.log('ml5 version:', ml5.version);
let brushColor = "black";
// sketch model;
let model;

// stroke
let strokePath;
let seedStroke = [];
let x = 0;
let y = 0;

// previous pen state
let previousPen = "down";

// canvas reference
let canvas;


let pHtmlMsg;
let serialOptions = { baudRate: 115200  };
let serial;

function setup(){
  canvas = createCanvas(1000, 1000);
  canvas.background(225);
  // Create a new SketchRNN Instance
  model = ml5.sketchRNN("bee", modelReady);
  canvas.mouseReleased(startSketchRNN);
  
  // Setup Web Serial using serial.js
  serial = new Serial();
  serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
  serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
  serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
  serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);

  // If we have previously approved ports, attempt to connect with them
  //serial.autoConnectAndOpenPreviouslyApprovedPort(serialOptions);

  // Add in a lil <p> element to provide messages. This is optional
  pHtmlMsg = createP("Click anywhere on this page to open the serial connection dialog");
  pHtmlMsg.style('color', 'deeppink');
}

function draw(){
  // draw starting form
  if (mouseIsPressed) {
    stroke(brushColor)
    strokeWeight(3.0);
    line(pmouseX, pmouseY, mouseX, mouseY);
    const curStroke = {
      dx : mouseX - pmouseX,
      dy : mouseY - pmouseY,
      pen : "down"
    };
    seedStroke.push(curStroke);
    x = mouseX;
    y = mouseY;
  }
  
  // when mouse release
  // strokePath will be intialized
  if (strokePath) {
    // draw generated stroke
    if (previousPen === "down") {
      stroke("red");
      strokeWeight(3.0);
      line(x, y, x + strokePath.dx, y + strokePath.dy);
    }
    
    // Move the pen
    x += strokePath.dx;
    y += strokePath.dy;
    previousPen = strokePath.pen;

    // check if drawing is complete
    if (strokePath.pen !== "end") {
      strokePath = null;
      model.generate(gotStroke);
    } else if (strokePath.pen === "end") {
      // reset model and seed
      console.log("done");
      strokePath = null;
      model.reset();
      seedStroke = [];
    }
    
  }

}

// callback function for model initialization
function modelReady() {
  console.log("model loaded");
}

// get next stroke
function gotStroke(err, s) {
  strokePath = s;
}

// generate first stroke
// called when mouse is first released after click
function startSketchRNN() {
  model.generate(seedStroke, gotStroke);
}


/**
 * Callback function by serial.js when there is an error on web serial
 * 
 * @param {} eventSender 
 */
 function onSerialErrorOccurred(eventSender, error) {
  console.log("onSerialErrorOccurred", error);
  pHtmlMsg.html(error);
}

/**
 * Callback function by serial.js when web serial connection is opened
 * 
 * @param {} eventSender 
 */
function onSerialConnectionOpened(eventSender) {
  console.log("onSerialConnectionOpened");
  pHtmlMsg.html("Serial connection opened successfully");
}

/**
 * Callback function by serial.js when web serial connection is closed
 * 
 * @param {} eventSender 
 */
function onSerialConnectionClosed(eventSender) {
  console.log("onSerialConnectionClosed");
  pHtmlMsg.html("onSerialConnectionClosed");
}

/**
 * Callback function serial.js when new web serial data is received
 * 
 * @param {*} eventSender 
 * @param {String} newData new data received over serial
 */
function onSerialDataReceived(eventSender, newData) {
  console.log("onSerialDataReceived", newData);
  if (newData === "clear") {
    clear();
    canvas.background(225);
  } else {
    pHtmlMsg.html("onSerialDataReceived: " + newData);
    model = ml5.sketchRNN(newData, modelReady);
    seedStroke = [];
  }
  
  
}

/**
 * Called automatically by the browser through p5.js when mouse clicked
 */
function mouseClicked() {
  if (!serial.isOpen()) {
    serial.connectAndOpen(null, serialOptions);
  }
}
