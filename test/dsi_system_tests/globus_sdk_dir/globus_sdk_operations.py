# $URL: svn+ssh://gregthor@jenova/var/svn/dsi/branches/2.1.0/test/dsi_system_tests/globus_sdk_dir/globus_sdk_operations.py $: $id:$ $Rev: 17 $
import globus_sdk
import logging


class globus_sdk_operations(object): 
    
    def __init__(self, authorizer):
        transfer_client = globus_sdk.TransferClient(authorizer=authorizer)
        self.tc = transfer_client

    def get_my_endpoints(self):
        my_endpoints = []
        for ep in self.tc.endpoint_search(filter_scope='my-endpoints'):
            endpoint = [ep['display_name'], ep['id']]
            my_endpoints.append(endpoint)
        return my_endpoints
    
    def ls_dir(self, endpoint_id, directory):
        entries = []
        for entry in self.tc.operation_ls(endpoint_id, path=directory):
            entries.append([entry['name'], entry['type']])
        return entries

    def file_exists(self, endpoint_id, directory, filename):
        return self.get_file_info(endpoint_id, directory, filename) != None

    def get_file_info(self, endpoint_id, directory, filename):
        entries = []
        for entry in self.tc.operation_ls(endpoint_id, path=directory):
            if entry['type'] == 'file' and entry['name'] == filename:
               return entry

    def dir_exists(self, endpoint_id, directory, sub_directory):
        entries = []
        for entry in self.tc.operation_ls(endpoint_id, path=directory):
            if entry['type'] == 'dir' and entry['name'] == sub_directory:
                return True
        return False
    
    def complete_task(self, task_id, timeout, polling_interval):
        return self.tc.task_wait(task_id, timeout, polling_interval)
    
    def get_task(self, task_id):
        return self.tc.get_task(task_id)
    
    def task_successful(self, task_id):
        task = self.get_task(task_id)
        return task['status'] == 'SUCCEEDED'

    def get_files_in_dir(self, endpoint_id, directory):
        entries = []
        for entry in self.tc.operation_ls(endpoint_id, path=directory):
            if entry['type'] == 'file':
                entries.append(entry)
        return entries

    def get_file_in_dir(self, endpoint_id, directory, filename):
        for entry in self.tc.operation_ls(endpoint_id, path=directory):
            if entry['type'] == 'file' and entry['name'] == filename:
                return entry
    
    def mkdir(self, endpoint_id, directory):
        self.tc.operation_mkdir(endpoint_id, directory)
    
    def recursive_delete_dir(self, endpoint_id, directory):
        logging.info('Recursivly delete dir ' + directory + ' on endpoint ' +
                endpoint_id)
        ddata = globus_sdk.DeleteData(self.tc, endpoint_id, recursive=True)
        ddata.add_item(directory+'/')
        delete_result = self.tc.submit_delete(ddata)
        if delete_result['code'] != 'Accepted':
            raise IOError('Delete failed')
        if self.tc.task_wait(delete_result['task_id'], 10, 1) != True:
            raise IOError('Delete failed')

        return delete_result

    def delete_file(self, endpoint_id, file_path):
        logging.info('Delete file ' + file_path + ' on endpoint ' +
                endpoint_id)
        ddata = globus_sdk.DeleteData(self.tc, endpoint_id, recursive=False)
        ddata.add_item(file_path)
        delete_result = self.tc.submit_delete(ddata)
        if delete_result['code'] != 'Accepted':
            raise IOError('Delete failed')
        if self.tc.task_wait(delete_result['task_id'], 10, 1) != True:
            raise IOError('Delete failed')
        
        return delete_result
    
