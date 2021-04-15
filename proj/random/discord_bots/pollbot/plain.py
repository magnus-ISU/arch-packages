import discord
import os

client = discord.Client()
pollnum = 1
pollnumbers = '🇦🇧🇨🇩🇪🇫🇬🇭🇮🇯🇰🇱🇲🇳🇴🇵🇶🇷🇸🇹🇺🇻🇼🇽🇾🇿'

@client.event
async def on_ready():
  print('Logged in as {0.user}'.format(client))

@client.event
async def on_message(m):
  if m.author == client.user:
    if m.content.startswith('Poll '):
      await reactpoll(m)
  if m.content.startswith('+poll'):
    await printpoll(m)
    return

async def reactpoll(m):
  lines = m.content.split('\n')
  for line in lines:
    if line.startswith(':'):
      emoji = line.split(':',2)[1]
      letter = emoji.split('_')[2]
      unicode = pollnumbers[ord(letter)-ord('a')]
      await m.add_reaction(unicode)
    elif not line[0].isascii():
      emoji = line[0]
      await m.add_reaction(emoji)

async def printpoll(m):
  global pollnum
  poll = 'Poll %d: ' % (pollnum)

  polltitle, str = getthingin(m.content, '{', '}')
  if len(polltitle) == 0:
    await pollerror(m, 'no title')
    return
  polloptions = []
  while True:
    cpy = str
    opt, str = getthingin(cpy, '[', ']')
    if len(opt) != 0 and len(polloptions) < 26:
      polloptions.append(opt)
    if len(str) == 0:
      break
  if len(polloptions) == 0:
    await pollerror(m, 'no options to vote on')
    return
  
  poll += '*** %s ***\n' % (polltitle)
  num = ord('a')
  for option in polloptions:
    if not option[0].isascii():
      poll += "%s\n" % (option)
    else:
      poll += ':regional_indicator_%s: %s\n' % (chr(num), option)
      num += 1

  await m.channel.send(poll)
  pollnum += 1

def getthingin(s, start, end):
  i = 0
  inTitle = False
  retval = ''
  while i < len(s):
    if s[i] == '\\' and inTitle:
      i+=1
      retval += s[i]
    elif s[i] == end and inTitle:
      break
    elif inTitle:
      retval += s[i]
    elif s[i] == start:
      inTitle = True
    i += 1
  return (retval.strip(), s[i+1:].strip())

client.run(os.getenv('DISCORD_TOKEN'))
