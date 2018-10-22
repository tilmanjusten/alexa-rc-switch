/*
 |--------------------------------------------------------------------------
 | Browser-sync config file
 |--------------------------------------------------------------------------
 |
 | For up-to-date information about the options:
 |   http://www.browsersync.io/docs/options/
 |
 | There are more options than you see here, these are just the ones that are
 | set internally. See the website for more info.
 |
 |
 */
const devices = [
  {
    'id': 0,
    'name': 'Alle Lichter',
    'name_id': 'alle-lichter'
  },
  {
    'id': 1,
    'name': 'Wohnzimmerlicht',
    'name_id': 'wohnzimmerlicht'
  },
  {
    'id': 2,
    'name': 'Sofa & Regal',
    'name_id': 'sofalicht'
  },
  {
    'id': 3,
    'name': 'Lichterkette',
    'name_id': 'lichterkette'
  },
  {
    'id': 4,
    'name': 'Tischregal',
    'name_id': 'tischregal'
  },
  {
    'id': 5,
    'name': 'Galerielicht',
    'name_id': 'galerielicht'
  }
]

const routesState = devices.map(device => {
  return {
    route: `/state/${device.name_id}`,
    handle: function (req, res, next) {
      res.end(`{ "device_id": "${device.id}", "device_name": "${device.name}", "device_name_id": "${device.name_id}", "state": "on" }`)

      next()
    }
  }
})

const routesSwitchOn = devices.map(device => {
  return {
    route: `/switch/${device.name_id}/on`,
    handle: function (req, res, next) {
      res.end(`{ "device_id": "${device.id}", "device_name": "${device.name}", "device_name_id": "${device.name_id}", "state": "on" }`)

      next()
    }
  }
})

const routesSwitchOff = devices.map(device => {
  return {
    route: `/switch/${device.name_id}/off`,
    handle: function (req, res, next) {
      res.end(`{ "device_id": "${device.id}", "device_name": "${device.name}", "device_name_id": "${device.name_id}", "state": "off" }`)

      next()
    }
  }
})

module.exports = {
  "ui": {
    "port": 3001,
    "weinre": {
      "port": 8080
    }
  },
  "files": false,
  "watchOptions": {},
  "server": "data/www",
  "proxy": false,
  "port": 3000,
  "middleware": [...routesState, ...routesSwitchOn, ...routesSwitchOff],
  "serveStatic": [],
  "ghostMode": {
    "clicks": true,
    "scroll": true,
    "forms": {
      "submit": true,
      "inputs": true,
      "toggles": true
    }
  },
  "logLevel": "info",
  "logPrefix": "BS",
  "logConnections": false,
  "logFileChanges": true,
  "logSnippet": true,
  "rewriteRules": false,
  "open": false,
  "browser": "default",
  "xip": false,
  "hostnameSuffix": false,
  "reloadOnRestart": false,
  "notify": true,
  "scrollProportionally": true,
  "scrollThrottle": 0,
  "scrollRestoreTechnique": "window.name",
  "scrollElements": [],
  "scrollElementMapping": [],
  "reloadDelay": 0,
  "reloadDebounce": 0,
  "reloadThrottle": 0,
  "plugins": [],
  "injectChanges": true,
  "startPath": null,
  "minify": true,
  "host": null,
  "codeSync": true,
  "timestamps": true,
  "clientEvents": [
    "scroll",
    "scroll:element",
    "input:text",
    "input:toggles",
    "form:submit",
    "form:reset",
    "click"
  ],
  "socket": {
    "socketIoOptions": {
      "log": false
    },
    "socketIoClientConfig": {
      "reconnectionAttempts": 50
    },
    "path": "/browser-sync/socket.io",
    "clientPath": "/browser-sync",
    "namespace": "/browser-sync",
    "clients": {
      "heartbeatTimeout": 5000
    }
  },
  "tagNames": {
    "less": "link",
    "scss": "link",
    "css": "link",
    "jpg": "img",
    "jpeg": "img",
    "png": "img",
    "svg": "img",
    "gif": "img",
    "js": "script"
  }
};
