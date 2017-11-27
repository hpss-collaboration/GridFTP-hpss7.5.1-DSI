import globus_sdk


class globus_sdk_endpoints(object):

    def __init__(self, authorizer):
        transfer_client = globus_sdk.TransferClient(authorizer=authorizer)
        self.tc = transfer_client

    def auto_activate(self, ep_id, if_expires_in=3600):
        r = self.tc.endpoint_autoactivate(ep_id, if_expires_in=3600)
        while (r['code'] == 'AutoActivationFailed'):
            print('Endpoint requires manual activation, please open ' +
                    'the following URL in a browser to activate the ' +
                    'endpoint:')
            print('https://www.globus.org/app/endpoints/' + ep_id + '/activate')
            raw_input('Press ENTER after activating the endpoint:')
            r = self.tc.endpoint_autoactivate(ep_id, if_expires_in=3600)

    def create_endpoint(self, display_name, hostname):
        ep_data = {
          'DATA_TYPE': 'endpoint', 
          'display_name': display_name, 
          'DATA': [
              {
                  'DATA_TYPE': 'server',
                  'hostname': hostname,
              },
            ],
          }

        create_result = self.tc.create_endpoint(ep_data)
        endpoint_id = create_result['id']
        return endpoint_id

    def delete_endpoint(self, endpoint_id):
        delete_result = self.tc.delete_endpoint(endpoint_id)
        print delete_result

