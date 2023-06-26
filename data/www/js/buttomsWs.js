const relays = [
  document.getElementById("buttom1"),
  document.getElementById("buttom2"),
  document.getElementById("buttom3"),
  document.getElementById("buttom4"),
];

const currentTimeMobile = document.getElementById("timeValueMobile");
const currentTimePc = document.getElementById("timeValuePc");

let gateway = `ws://${window.location.hostname}/ws`;
let websocket;

const onOpen = () => {
  console.log("Connection opened");
};

const onClose = () => {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
};

const onToggle = (result) => {
  if (result.action !== "toggle") return;
  const state = result.state ? "on" : "off";
  relays[result.relay].className = state;
};

const onTime = (result) => {
  if (result.action !== "time") return;
  currentTimeMobile.innerHTML = `Current Time: ${result.now}`;
  currentTimePc.innerHTML = `Current Time: ${result.now}`;
};

const onMessage = (event) => {
  const result = JSON.parse(event.data);
  onToggle(result);
  onTime(result);
};

function initWebSocket() {
  console.log("Trying to open a WebSocket connection...");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage; // <-- add this line
}

const toggle = (relay) => {
  websocket.send(
    JSON.stringify({
      relay,
      action: "toggle",
    })
  );
};

const initButton = () => {
  relays[0].addEventListener("click", () => {
    toggle(0);
  });

  relays[1].addEventListener("click", () => {
    toggle(1);
  });

  relays[2].addEventListener("click", () => {
    toggle(2);
  });

  relays[3].addEventListener("click", () => {
    toggle(3);
  });
};

const onLoad = () => {
  initWebSocket();
  initButton();
};

window.addEventListener("load", onLoad);
