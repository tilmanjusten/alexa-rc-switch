(function() {
  const button = document.querySelector('.switch--alle-lichter')
  const xhr = new XMLHttpRequest()

  xhr.open("GET", "http://lichter.fritz.box/state")
  xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
  xhr.onload = function () {
    if (xhr.status === 200 && xhr.responseText === "on") {
      button.classList.remove("switch--off")
      button.classList.add("switch--on")
    } else {
      button.classList.remove("switch--on")
      button.classList.add("switch--off")
    }
  }

  xhr.send()
})()

function sendSwitch(button) {
  const state = button.classList.contains("switch--on")
  const current_state = state ? "on" : "off"
  const target_state = state ? "off" : "on"
  const device_name = button.dataset.name
  const xhr = new XMLHttpRequest()

  xhr.open("GET", "/switch/${device_name}/${target_state}")
  xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
  xhr.onload = function () {
    if (xhr.status === 200 && xhr.responseText === target_state) {
      button.classList.remove("switch--${current_state}")
      button.classList.add("switch--${target_state}")
    }
  }
  xhr.send()
}
