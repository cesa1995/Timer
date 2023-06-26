const lanES = document.getElementById("lanES");
const lanEN = document.getElementById("lanEN");
const syncTimeW = document.getElementById("syncTimeW");
const syncTime = document.getElementById("syncTime");

const LoadData = (RelaysData) => {
  const lan = RelaysData.lan;
  if (lan === "EN") {
    lanES.className = "lanButtom lanButtomDisabled";
  }
  if (lan === "ES") {
    lanEN.className = "lanButtom lanButtomDisabled";
  }
};

const syncTimeFromBrowser = () => {
  const now = new moment();
  const time = now.unix() + now.utcOffset() * 60;
  fetch("/config", {
    headers: {
      "Content-Type": "application/json",
    },
    method: "POST",
    body: JSON.stringify({
      now: time,
    }),
  }).then((response) => {
    if (response.ok) {
      window.location.href = `/`;
    }
  });
};

syncTimeW.addEventListener("click", () => {
  syncTimeFromBrowser();
});

syncTime.addEventListener("click", () => {
  syncTimeFromBrowser();
});

lanEN.addEventListener("click", () => {
  fetch("/config", {
    headers: {
      "Content-Type": "application/json",
    },
    method: "POST",
    body: JSON.stringify({
      lan: "EN",
    }),
  }).then((response) => {
    if (response.ok) {
      window.location.href = `/`;
    }
  });
});

lanES.addEventListener("click", () => {
  fetch("/config", {
    headers: {
      "Content-Type": "application/json",
    },
    method: "POST",
    body: JSON.stringify({
      lan: "ES",
    }),
  }).then((response) => {
    if (response.ok) {
      window.location.href = `/`;
    }
  });
});
