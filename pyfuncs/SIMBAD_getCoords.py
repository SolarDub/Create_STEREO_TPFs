import warnings
warnings.filterwarnings('ignore')
import sys
import astropy
from astroquery.simbad import Simbad

star = str(sys.argv[1])

t = Simbad.query_object(star)

for ra, dec in t.iterrows('RA', 'DEC'):
    print("\nStar:\t{}\nRA:\t{}\nDEC:\t{}\n".format(star,ra,dec))
    
print(ra)
print(dec)

coords = [ra, dec]

sys.exit(coords)
