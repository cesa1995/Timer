const AccessPointMenuButtom = document.getElementById("accessPointMenu");
const WifiMenuButtom = document.getElementById("wifiMenu");
const HomeMenuButtom = document.getElementById("homeMenu");
const Relay1menuButtom = document.getElementById("relay1Menu");
const Relay2menuButtom = document.getElementById("relay2Menu");
const Relay3menuButtom = document.getElementById("relay3Menu");
const Relay4menuButtom = document.getElementById("relay4Menu");

const hamburger = document.getElementById("hamburger");
const menu = document.getElementById("menu");

fetch("/config.json")
  .then((res) => res.json())
  .then(LoadData);

const goAccessPoint = () => {
  window.location.href = "/ap/";
};

const goWifi = () => {
  window.location.href = "/wifi/";
};

const goHome = () => {
  window.location.href = "/";
};

const goRelay = (relayNumberArg) => {
  window.location.href = `/relay/?number=${relayNumberArg}`;
};

AccessPointMenuButtom.addEventListener("click", () => {
  goAccessPoint();
});

WifiMenuButtom.addEventListener("click", () => {
  goWifi();
});

HomeMenuButtom.addEventListener("click", () => {
  goHome();
});

Relay1menuButtom.addEventListener("click", () => {
  goRelay(0);
});

Relay2menuButtom.addEventListener("click", () => {
  goRelay(1);
});

Relay3menuButtom.addEventListener("click", () => {
  goRelay(2);
});

Relay4menuButtom.addEventListener("click", () => {
  goRelay(3);
});

hamburger.addEventListener("click", () => {
  if (menu.style.display === "flex") {
    menu.style.display = "none";
    return;
  }
  menu.style.display = "flex";
});
