import configparser
import argparse
import requests
import socket
import json
import time
import sys
import re
import HTMLParser
import datetime

from pprint import pprint

def enqueue(server, filename, num_matches=150):
    if server[-1] == "/":
        server = server[:-1]

    server = server.replace("http://", "")
    server = server.replace("https://", "")

    fkey = ''
    # obtains the form key
    try:
        server_uri = "http://" + server + "/ga_server/experiment/all_records/new/experiment"
        r = requests.get(server_uri, timeout=5)


        #pprint(vars(r))
        #print r.content
        fkey = re.match(r'.*_formkey.*value="(.*)" /><input', r.content, re.M | re.I | re.DOTALL).group(1)
        sys.stderr.write(fkey)
        #decoded = json.loads(r.content)

    except requests.exceptions.Timeout:
        print 'Timeout exception'

    except requests.exceptions.RequestException as e:
        print "Request exception :", str(e)

    time.sleep(1)
    cfg = configparser.ConfigParser(filename)

    payload = {
        'experiment_xml_data': open(filename).read(),
        'experiment_num_matches': num_matches,
        'experiment_name': cfg.output_dir,
        'experiment_status': 'READY',
        '_formname': 'web2py_grid',
        '_formkey': fkey
    }

    try:
        server_uri = "http://" + server + "/ga_server/experiment/all_records/new/experiment"
        r = requests.post(server_uri, data=payload, timeout=5)
        #pprint(vars(r))
        print r.content
        #decoded = json.loads(r.content)

    except requests.exceptions.Timeout:
        print 'Timeout exception'

    except requests.exceptions.RequestException as e:
        print "Request exception :", str(e)

'''
    if "success" in decoded and decoded["success"] == "true":
        print 'File %s enqueued successfully' % filename
    else:
        print 'An error has occurred: ', decoded["message"]

'''
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Enqueue experiments in the server')

    parser.add_argument('server', help="The server to enqueue experiments")
    parser.add_argument('experiment', nargs='+', help="The experiment to enqueue")

    args = parser.parse_args()

    for e in args.experiment:
        enqueue(args.server, e)
        time.sleep(2)

