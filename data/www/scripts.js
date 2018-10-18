const devices = [{
  'id': 0,
  'name': 'Alle Lichter',
  'name_id': 'alle-lichter'
}, {
  'id': 1,
  'name': 'Wohnzimmerlicht',
  'name_id': 'wohnzimmerlicht'
}, {
  'id': 2,
  'name': 'Sofa & Regal',
  'name_id': 'sofalicht'
}, {
  'id': 3,
  'name': 'Lichterkette',
  'name_id': 'lichterkette'
}, {
  'id': 4,
  'name': 'Türlicht',
  'name_id': 'tuerlicht'
}, {
  'id': 5,
  'name': 'Galerielicht',
  'name_id': 'galerielicht'
}];

// Get current state
(function () {
  devices.forEach(device => {
    const button = document.querySelector(`.switch--${device.name_id}`)
    const xhr = new XMLHttpRequest()

    xhr.open('GET', `/state/${device.name_id}`)
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded')
    xhr.onload = function () {
      if (xhr.status === 200) {
        const response = JSON.parse(xhr.response)
        const removeClassModifier = response.state === 'on' ? 'off' : 'on';

        button.classList.remove(`switch--${removeClassModifier}`)
        button.classList.add(`switch--${response.state}`)
      } else {
        console.error('Something went wrong getting the state of %s', device.name)
        console.table(device)
        console.dir(xhr)
      }
    }

    xhr.send()
  })
})()

// Toggle switches on user interaction
const toggleSwitch = evt => {
  const button = evt.target
  const state = button.classList.contains('switch--on')
  const targetState = state ? 'off' : 'on'
  const deviceName = button.dataset.name
  const xhr = new XMLHttpRequest()

  xhr.open('GET', `/switch/${deviceName}/${targetState}`)
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded')
  xhr.onload = function () {
    if (xhr.status === 200) {
      const response = JSON.parse(xhr.response)
      const removeClassModifier = response.state === 'on' ? 'off' : 'on';

      button.classList.remove(`switch--${removeClassModifier}`)
      button.classList.add(`switch--${response.state}`)
    } else {
      console.error('Something went wrong when setting the state of %s', device.name)
      console.table(button)
      console.dir(xhr)
    }
  }
  xhr.send()
}

document.querySelectorAll('.switch').forEach(button => {
  button.addEventListener('click', toggleSwitch)
})
