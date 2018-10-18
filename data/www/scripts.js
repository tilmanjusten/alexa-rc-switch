const devices = [{
  'id': 0,
  'name': 'Alle Lichter',
  'name_id': 'alle-lichter',
  'state': 'off',
  'dependents': [
    'wohnzimmerlicht',
    'sofalicht',
    'lichterkette',
    'tuerlicht',
    'galerielicht'
  ]
}, {
  'id': 1,
  'name': 'Wohnzimmerlicht',
  'name_id': 'wohnzimmerlicht',
  'state': 'off',
  'dependents': [
    'sofalicht',
    'lichterkette'
  ]
}, {
  'id': 2,
  'name': 'Sofa & Regal',
  'name_id': 'sofalicht',
  'state': 'off'
}, {
  'id': 3,
  'name': 'Lichterkette',
  'name_id': 'lichterkette',
  'state': 'off'
}, {
  'id': 4,
  'name': 'Türlicht',
  'name_id': 'tuerlicht',
  'state': 'off'
}, {
  'id': 5,
  'name': 'Galerielicht',
  'name_id': 'galerielicht',
  'state': 'off'
}];

const updateStateDependencies = (deviceIndex) => {
  const device = devices[deviceIndex]

  // update dependents
  if (device.hasOwnProperty('dependents')) {
    device.dependents.forEach(dependencyNameId => {
      const dependencyIndex = devices.findIndex(device => device.name_id === dependencyNameId)

      devices[dependencyIndex].state = device.state
    })
  }

  // get parents
  let parents = devices.filter(dep => dep.hasOwnProperty('dependents') && dep.dependents.includes(device.name_id))

  // loop parents and check if dependents have same state
  parents.forEach((parent, parentIndex) => {
    let childStates = parent.dependents.map(depNameId => {
      let state = devices.find(child => child.name_id === depNameId).state

      return state === 'on' ? 1 : 0
    })

    // parent is "on" if all dependents are enabled
    state = childStates.reduce((prev, cur) => prev + cur)

    devices[parentIndex].state = state === parent.dependents.length ? 'on' : 'off'
  })
}

const updateUI = () => {
  devices.forEach(device => {
    const button = document.querySelector(`.switch--${device.name_id}`)
    const removeClassModifier = device.state === 'on' ? 'off' : 'on'

    button.classList.remove(`switch--${removeClassModifier}`)
    button.classList.add(`switch--${device.state}`)
  })
}

// Get current state
(function () {
  devices.forEach(device => {
    const xhr = new XMLHttpRequest()

    xhr.open('GET', `/state/${device.name_id}`)
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded')
    xhr.onload = function () {
      if (xhr.status === 200) {
        const response = JSON.parse(xhr.response)

        device.state = response.state

        updateUI()
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
const switchOffByDeviceNameId = name_id => {
  const deviceIndex = devices.findIndex(device => device.name_id === name_id)

  devices[deviceIndex].state = 'off'
}

const toggleSwitch = evt => {
  const button = evt.target
  const state = button.classList.contains('switch--on')
  const targetState = state ? 'off' : 'on'
  const deviceName = button.dataset.name
  const xhr = new XMLHttpRequest()
  const deviceIndex = devices.findIndex(device => device.name_id === deviceName)

  xhr.open('GET', `/switch/${deviceName}/${targetState}`)
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded')
  xhr.onload = function () {
    if (xhr.status === 200) {
      const response = JSON.parse(xhr.response)

      devices[deviceIndex].state = response.state

      updateStateDependencies(deviceIndex)

      updateUI()
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
