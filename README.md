# Snowcrash

Snow Crash is a developer-oriented cybersecurity exercise at 42: you explore a VM with progressively advanced levels, uncover vulnerabilities (buffer overflows, shell exploits, mis-configurations), and document flags and techniques.<br>
This repo contains my write-ups and reflections on the exploit challenges.

## Snowcrash VM Setup Guide

### Goal

Download the Snowcrash ISO, create a VirtualBox VM, attach the ISO, and configure a bridged network so the VM and your host appear on the same LAN (same subnet) and can reach each other directly.

To download SnowCrash iso: https://cdn.intra.42.fr/isos/SnowCrash.iso

<!-- ### Configuring a Bridged Network (VirtualBox) — Quick Instructions

Follow these steps to place the VM and host on the same LAN/subnet.

#### Configuration Steps

1. **Open VirtualBox → Settings** (for your VM where you installed Snowcrash.iso image) → **Network**

2. **Adapter 1** → Enable Network Adapter
   - **Attached to:** Bridged Adapter
   - **Name:** Select your host's physical interface (e.g., eth0, enp3s0, Wi-Fi)
   - **Promiscuous Mode:** Deny (or Allow VMs if needed)
   - **Cable Connected:** Checked

3. **Click OK/Save**

4. **Start the VM**

5. **Access the Snowcrash_IP_Adress in your web browser**

<p align="center">
  <img src="prerequisites/Snowcrash_img.png" width="800">
</p>

#### Network Configuration

- If the VM uses DHCP, it should obtain an IP from the same network as the host.
- If necessary, configure a static IP in the VM (gateway and netmask must match the host network).

> [!TIP]
> Use the exact same network for the local machine and the VM. Don't use a VPN. <br>
> Access the site in **HTTP**. -->

## Tools I used:

