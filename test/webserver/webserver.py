#!/usr/bin/env python3
import cherrypy
import os
import time


class Root(object):
    def __init__(self):
        self._switchOn = False
    @cherrypy.expose
    def index(self):
        with open('../../html/index.html', 'rt') as webpage:
            return webpage.read()
    @cherrypy.expose
    def state(self, state=None):
        if cherrypy.request.method == 'POST':
            if state is None:
                raise cherrypy.HTTPError(status=400, message='"state" argument was not specified')
            if state.upper() == 'ON':
                print('Turning switch on')
                self._switchOn = True
            elif state.upper() == 'OFF':
                print('Turning switch off')
                self._switchOn = False
            else:
                raise cherrypy.HTTPError(status=400, message='Invalid value for argument "state": must be either "on" or "off".')
            time.sleep(0.25)
            return 'OK'
        elif cherrypy.request.method == 'GET':
            print('Switch is %s' % ('ON' if self._switchOn else 'OFF'))
            return 'ON' if self._switchOn else 'OFF'
        else:
            raise cherrypy.HTTPError(status=405, message='Method %s not supported' % cherrypy.request.method)
    

if __name__ == "__main__":
    conf = {
        '/static': {
            'tools.staticdir.on': True,
            'tools.staticdir.dir': os.path.abspath('./public')
        },
        '/': {
            'tools.sessions.on': True
        }
    }
    cherrypy.config.update({'server.socket_port': 8081})
    cherrypy.server.socket_host = '0.0.0.0'
    cherrypy.quickstart(Root(), '/', conf)
