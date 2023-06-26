const ssid = document.getElementById("ssid");
const password = document.getElementById("password");

const LoadData = (RelaysData) => {
  ssid.innerHTML = RelaysData.credentialsWifi.ssid;
  password.innerHTML = RelaysData.credentialsWifi.password;
};

const onHandleWifiForm = async (event) => {
  event.preventDefault();
  const data = new FormData(event.target);
  const value = Object.fromEntries(data.entries());
  if (!value.ssid) {
    return;
  }
  const result = await fetch("/config", {
    headers: {
      "Content-Type": "application/json",
    },
    method: "POST",
    body: JSON.stringify(value),
  }).then((result) => result.json());
  if (result.data) {
    window.location.href = "/wifi/";
  }
};

this.onChangeWifiCredentials = document.getElementById(
  "onChangeWifiCredentials"
);
this.onChangeWifiCredentials.addEventListener(
  "submit",
  async (props) => await onHandleWifiForm(props)
);
