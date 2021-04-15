import discord
import os

client = discord.Client()
pollnum = 1
pollnumbers = '🇦🇧🇨🇩🇪🇫🇬🇭🇮🇯🇰🇱🇲🇳🇴🇵🇶🇷🇸🇹🇺🇻🇼🇽🇾🇿'
link = 'https://www.youtube.com/watch?v=dQw4w9WgXcQ'

@client.event
async def on_ready():
  print('Logged in as {0.user}'.format(client))

@client.event
async def on_message(m):
  if m.author == client.user:
    #await reactpoll(m, m.content) # if you want simple messages instead of embeds
    if len(m.embeds) == 1:
      await reactpoll(m, m.embeds[0].description)
  if m.content.startswith('+poll'):
    await printpoll(m)
    return

async def reactpoll(m, s):
  lines = s.split('\n')
  for line in lines:
    if len(line) == 0:
      continue
    if line.startswith(':'):
      emoji = line.split(':',2)[1]
      letter = emoji.split('_')[2]
      unicode = pollnumbers[ord(letter)-ord('a')]
      await m.add_reaction(unicode)
    elif not line[0].isascii():
      emoji = line[0]
      await m.add_reaction(emoji)

async def printpoll(m):
  global pollnum, lastmsg
  poll = 'Poll %d: ' % (pollnum)
  desc = ''

  polltitle, str = getthingin(m.content, '{', '}')
  if len(polltitle) == 0:
    await pollerror(m, 'no title')
    return
  polloptions = []
  while True:
    cpy = str
    opt, str = getthingin(cpy, '[', ']')
    if len(opt) != 0 and len(polloptions) < 20:
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
      desc += "%s\n" % (option)
    else:
      desc += ':regional_indicator_%s: %s\n' % (chr(num), option)
      num += 1
  #await m.channel.send(poll + desc) # For simpler messages without embeds
  embed = discord.Embed(title=poll, 
      url=link,
      description=desc,
      color = discord.Color.blue())
  embed.set_author(name = m.author.display_name,
      url = link,
      icon_url = m.author.avatar_url)
  embed.set_footer(text = "---------------------- One out of ten hackers agree - binary is better! ----------------------")
  
  await m.channel.send(embed=embed)
  pollnum += 1

async def pollerror(m, errmsg):
  await m.channel.send("Error making the poll: %s" % (errmsg))

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
