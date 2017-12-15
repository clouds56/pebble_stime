// Function to send a message to the Pebble using AppMessage API
// We are currently only sending a message using the "status" appKey defined in appinfo.json/Settings
function sendMessage(msg) {
  Pebble.sendAppMessage(msg, messageSuccessHandler, messageFailureHandler);
}

// Called when the message send attempt succeeds
function messageSuccessHandler() {
  console.log("Message send succeeded.");
}

// Called when the message send attempt fails
function messageFailureHandler(data) {
  console.log("Message send failed.");
  sendMessage(data);
}

// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
  console.log("JS is ready!");
  sendMessage({"status": 1});
});

// Called when incoming message from the Pebble is received
// We are currently only checking the "message" appKey defined in appinfo.json/Settings
Pebble.addEventListener("appmessage", function(e) {
  console.log("Received Message: " + e.payload.message);
  if (e.payload.message == "query message") {
    sendMessage({"message": "Hello, Pebble!"});
  }
});
