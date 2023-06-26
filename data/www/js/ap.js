const ssid = document.getElementById("ssidAp");
const password = document.getElementById("passwordAp");

const LoadData = (RelaysData) => {
  ssid.innerHTML = RelaysData.credentialsAp.ssid;
  password.innerHTML = RelaysData.credentialsAp.password;
};

const onHandleWifiForm = async (event) => {
  event.preventDefault();
  const data = new FormData(event.target);
  const value = Object.fromEntries(data.entries());
  if (!value.ssidAp) {
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
    window.location.href = "/ap/";
  }
};

this.onChangeApCredentials = document.getElementById("onChangeApCredentials");

this.onChangeApCredentials.addEventListener(
  "submit",
  async (props) => await onHandleWifiForm(props)
);
