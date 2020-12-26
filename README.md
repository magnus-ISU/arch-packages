# File documenting things I want to do on a fresh arch install

loadkeys dvorak
fdisk -l
fdisk /dev/sda
g
n 1 \n +550M
n 2 \n +2G
n 3 \n \n
t 1 1
t 2 19
w
fdisk /dev/sdb
g
n 1 \n \n
w

mkfs.fat -F32 /dev/sda1
mkswap /dev/sda2
swapon /dev/sda2
mkfs.ext4 /dev/sda3
mkfs.ext4 /dev/sdb1

mount /dev/sda3 /mnt
mount /dev/sdb1 /mnt/home
mkdir /mnt/boot/efi
mount /dev/sda1 /mnt/boot/efi

#Here connection to the internet is required. Have not tested this so I pray it will work
iwctl # is the program to connect to the internet
[iwd]# help
[iwd]# device list
[iwd]# station device scan
[iwd]# station device get-networks
[iwd]# station device connect SSID # will prompt for password

pacstrap /mnt base linux linux-firmware
genfstab -U /mnt >> /mnt/etc/fstab

arch-chroot /mnt

ln -sf /usr/share/zoneinfo/America/Chicago /etc/localtime
hwclock --systohc

packman -S nano
nano /etc/locale.gen
#uncomment en_US.UTF-8
locale-gen

echo "IuseARCHbtw" > /etc/hostname
#nano /etc/hosts and add the following
127.0.0.1	localhost
::1		localhost
127.0.1.1	IuseARCHbtw.localdomain		IuseARCHbtw

passwd
useradd -m mag
passwd mag
usermod -aG wheel,audio,video,optical,storage mag

pacman -S sudo
EDITOR=nano visudo # Uncomment %wheel line

pacman -S grub efibootmgr dosfstools os-prober mtools
grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=GRUB --recheck
grub-mkconfig -o /boot/grub/grub.cfg

pacman -S networkmanager
systemctl enable NetworkManager

# Default to dvorak: 
localectl set-keymap --no-convert dvorak

exit # to chroot
umount /mnt/home
umount /mnt/boot/efi
umount /mnt
shutdown now
# Now good to remove installation medium

# faillock editing to prevent lockout for derping my password
sudo nano /etc/security/faillock.conf
# change deny = 3 to deny = 99

# install dependencies for dwm and also just useful shit
sudo pacman -S xorg-server xorg-xinit xorg-xrandr xorg-xsetroot firefox nitrogen picom wget man make gcc git pkg-config
#1.2G of files wow

# 
cp /etc/X11/xinit/xinitrc .xinitrc
# Remove the last five lines and add
nano .xinitrc
#setxkbmap dvorak &
#xrandr --output Virtual-1 --mode 1920x1080 &
#picom -f &
#exec dwm

nano .bash_profile
# [[$(fgconsole 2>/dev/null) == 1 ]] && exec startx -- vt1

sudo ln -s /usr/include/freetype2/freetype /usr/include/freetype
sudo ln -s /usr/include/freetype2/ft2build.h /usr/include/ft2build.h

wget https://dl.suckless.org/dwm/dwm-6.2.tar.gz
wget https://dl.suckless.org/st/st-0.8.3.tar.gz
wget https://dl.suckless.org/tools/dmenu-4.9.tar.gz

tar -xzvf dwm-6.2.tar.gz
tar -xzvf st-0.8.3.tar.gz
tar -xzvf dmenu-4.9.tar.gz

sudo make -C dwm-6.2 clean install
sudo make -C st-0.8.3 clean install
sudo make -C dmenu-4.9 clean install

# start the X window manager
startx

# check timedatectl
timedatectl status
# maybe try timedatectl set-ntp true
# or read man pages
