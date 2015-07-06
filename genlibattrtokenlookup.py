#!/usr/bin/env python

TOKENS = [
    "dynamic",
    "length",
    "location",
    "maxconnections",
    "mediatype",
    "name",
    "origin",
    "piece",
    "preference",
    "priority",
    "type",
    "url",
]

def to_enum_hd(k):
    res = 'METALINK_ATTR_TOKEN_'
    for c in k.upper():
        if c == ':' or c == '-':
            res += '_'
            continue
        res += c
    return res

def build_header(headers):
    res = {}
    for k in headers:
        size = len(k)
        if size not in res:
            res[size] = {}
        ent = res[size]
        c = k[-1]
        if c not in ent:
            ent[c] = []
        ent[c].append(k)

    return res

def gen_enum():
    name = ''
    print 'typedef enum {'
    for k in TOKENS:
        print '  {},'.format(to_enum_hd(k))
    print '  METALINK_ATTR_TOKEN_MAX'
    print '} metalink_attr_token;'

def gen_index_header():
    print '''\
int metalink_lookup_attr_token(const char *name, size_t namelen) {
  switch (namelen) {'''
    b = build_header(TOKENS)
    for size in sorted(b.keys()):
        ents = b[size]
        print '''\
  case {}:'''.format(size)
        print '''\
    switch (name[{}]) {{'''.format(size - 1)
        for c in sorted(ents.keys()):
            headers = sorted(ents[c])
            print '''\
    case '{}':'''.format(c)
            for k in headers:
                print '''\
      if (lstreq("{}", name, {})) {{
        return {};
      }}'''.format(k[:-1], size - 1, to_enum_hd(k))
            print '''\
      break;'''
        print '''\
    }
    break;'''
    print '''\
  }
  return -1;
}'''

if __name__ == '__main__':
    gen_enum()
    print ''
    gen_index_header()
