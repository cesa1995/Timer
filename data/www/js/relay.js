const delayStart = document.getElementById("delayStart");
const delayWork = document.getElementById("delayWork");
const delayEnd = document.getElementById("delayEnd");

const relayNumber = document.getElementById("relayNumber");
const relayNumberButtom = document.getElementById("relayNumberButtom");

const relayTitle = document.getElementById("relayTitle");
const relayTitleWhite = document.getElementById("relayTitleWhite");
const queryString = window.location.search;
const urlParams = new URLSearchParams(queryString);

const onButtomConfig = document.getElementById("onButtomConfig");
const onAddTimer = document.getElementById("onAddTimer");

const tempConfig = document.getElementById("tempConfig");
const buttomConfig = document.getElementById("buttomConfig");

const openElement = (element, currentElement, state) => {
  if (currentElement > 5) {
    return;
  }
  element.style.display = state;
  openElement(element.nextElementSibling, currentElement + 1, state);
};

const addNameClickEvenListener = (container, index) => {
  if (index > 0) {
    container.style.display = "none";
    return;
  }
  container.className = "nameTimer";
  container.addEventListener("click", () => {
    if (container.nextElementSibling.style.display === "flex") {
      openElement(container.nextElementSibling, 0, "none");
      return;
    }
    openElement(container.nextElementSibling, 0, "flex");
  });
};

const createValueElement = ({
  container,
  value,
  indexObj,
  index,
  relayNumber,
}) => {
  if (indexObj < 5) {
    const nameValue = document.createElement("label");
    if (indexObj > 0 && indexObj < 3) {
      const now = moment();
      nameValue.innerHTML = moment
        .unix(value - now.utcOffset() * 60)
        .format("HH:mm");
    } else {
      nameValue.innerHTML = value;
    }
    nameValue.className = "timeText";
    container.append(nameValue);
    return;
  }
  const nameValue = document.createElement("label");
  nameValue.className = "switch";
  const switchInput = document.createElement("input");
  switchInput.type = "checkbox";
  switchInput.addEventListener("change", () => {
    fetch("/config", {
      headers: {
        "Content-Type": "application/json",
      },
      method: "POST",
      body: JSON.stringify({
        relay: relayNumber,
        index,
        checked: switchInput.checked,
      }),
    }).then(() => (window.location.href = `/relay/?number=${relayNumber}`));
  });
  switchInput.checked = value;
  nameValue.append(switchInput);
  const spanElement = document.createElement("span");
  spanElement.className = "slider round";
  nameValue.append(spanElement);
  container.append(nameValue);
};

const createElementInfo = ({
  infoContainer,
  key,
  value,
  indexObj,
  index,
  relayNumber,
}) => {
  const container = document.createElement("div");
  container.className = "time";
  if (key !== "Name") {
    const nameTitle = document.createElement("label");
    nameTitle.innerHTML = `${key}: `;
    nameTitle.className = "timeTitle";
    container.append(nameTitle);
  }

  createValueElement({ container, value, indexObj, index, relayNumber });
  addNameClickEvenListener(container, indexObj);

  infoContainer.insertBefore(
    container,
    infoContainer.children[infoContainer.children.length - 5]
  );
};

const createElementByKey = (infoContainer, info, index, relayNumber) => {
  Object.keys(info).forEach((key, indexObj) => {
    createElementInfo({
      infoContainer,
      key,
      value: info[key],
      indexObj,
      index,
      relayNumber,
    });
  });
  const deleteButtom = document.createElement("button");
  deleteButtom.innerHTML = "Delete";
  deleteButtom.className = "deleteButtom";
  deleteButtom.addEventListener("click", () => {
    fetch("/config", {
      headers: {
        "Content-Type": "application/json",
      },
      method: "POST",
      body: JSON.stringify({
        relay: relayNumber,
        index,
      }),
    }).then(() => (window.location.href = `/relay/?number=${relayNumber}`));
  });
  infoContainer.insertBefore(
    deleteButtom,
    infoContainer.children[infoContainer.children.length - 5]
  );
};

const LoadData = (RelaysData) => {
  const relayNumberArg = +urlParams.get("number");
  const relayName = RelaysData.lan === "EN" ? "Relay" : "Relé";
  relayTitleWhite.innerHTML = `${relayName}${relayNumberArg + 1}`;
  relayTitle.innerHTML = `${relayName}${relayNumberArg + 1}`;
  relayNumber.value = relayNumberArg;
  relayNumberButtom.value = relayNumberArg;
  const relayData = RelaysData.Data[relayNumberArg];
  const infoContainer = document.getElementById("infoContainer");

  if (infoContainer.children.length > 7) {
    let deleElement = infoContainer.children.length - 6;
    while (infoContainer.children.length > 7) {
      infoContainer.removeChild(infoContainer.children[deleElement]);
      deleElement = deleElement - 1;
    }
  }

  relayData.timer.forEach((info, index) => {
    createElementByKey(infoContainer, info, index, relayNumberArg);
  });

  delayStart.innerHTML = relayData.buttom["Manual Start Delay"];
  delayWork.innerHTML = relayData.buttom["Manual Work Delay"];
  delayEnd.innerHTML = relayData.buttom["Manual End Delay"];
};

const formatTimerData = (value) => {
  if (!value.start || !value.name) return;
  const now = new moment();
  const start = moment(value.start, "HH:mm").unix() + now.utcOffset() * 60;
  const intervalEnd = +value.intervalEnd;
  const intervalStart = +value.intervalStart;
  const end = moment(value.end, "HH:mm").unix() + now.utcOffset() * 60;
  const relay = +value.relay;
  return { start, intervalEnd, intervalStart, end, relay, name: value.name };
};

const formatButtomConfigData = (value) => {
  if (!value.delayEnd) return;
  const delayEnd = +value.delayEnd;
  const delayStart = +value.delayStart;
  const delayWork = +value.delayWork;
  const relay = +value.relay;
  return { delayEnd, delayStart, delayWork, relay };
};

const formatData = (value) => {
  return formatButtomConfigData(value) ?? formatTimerData(value) ?? undefined;
};

const onHandleForm = async (event) => {
  event.preventDefault();
  const data = new FormData(event.target);
  const value = Object.fromEntries(data.entries());
  const filter = formatData(value);
  if (!filter) return;
  const result = await fetch("/config", {
    headers: {
      "Content-Type": "application/json",
    },
    method: "POST",
    body: JSON.stringify(filter),
  }).then((result) => result.json());
  if (result.data) {
    window.location.href = `/relay/?number=${value.relay}`;
  }
};

onButtomConfig.addEventListener(
  "submit",
  async (props) => await onHandleForm(props)
);

onAddTimer.addEventListener(
  "submit",
  async (props) => await onHandleForm(props)
);

buttomConfig.addEventListener("click", () => {
  if (
    buttomConfig.nextElementSibling.style.display === "none" ||
    !buttomConfig.nextElementSibling.style.display
  ) {
    buttomConfig.nextElementSibling.style.display = "flex";
    buttomConfig.children[0].textContent = "<";
    return;
  }
  buttomConfig.nextElementSibling.style.display = "none";
  buttomConfig.children[0].textContent = ">";
});

tempConfig.addEventListener("click", () => {
  if (
    tempConfig.nextElementSibling.style.display === "none" ||
    !tempConfig.nextElementSibling.style.display
  ) {
    tempConfig.nextElementSibling.style.display = "flex";
    tempConfig.children[0].textContent = "<";
    return;
  }
  tempConfig.nextElementSibling.style.display = "none";
  tempConfig.children[0].textContent = ">";
});
